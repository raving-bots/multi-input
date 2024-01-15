// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.Linq;
using System.Runtime.InteropServices;

using JetBrains.Annotations;

using UnityEngine;

namespace RavingBots.MultiInput
{
	/// <summary>
	///     The main component of the library. You need to add this to every scene that
	///     uses components that query input. You should also give it priority over everything
	///     else in the script execution order settings.
	/// </summary>
	/// <remarks>
	///     <note type="important">
	///         There should ever be only one alive instance of this component, and the instance must
	///         outlive all active <see cref="IDevice" /> objects. Having multiple active instances is unsupported and might
	///         cause issues. If you don't have special requirements you should leave <see cref="LifetimeManagement" /> set to
	///         <c>true</c>.
	///     </note>
	///     <para>
	///         This component initializes and controls the native code used to query the OS for state of devices
	///         and their virtual axes. All of the currently detected devices can be found through the <see cref="Devices" />
	///         property.
	///         Additionally, you can subscribe to the <see cref="DeviceStateChanged" /> event to be notified when
	///         a new device is plugged in, or an old device is unplugged.
	///     </para>
	///     <para>
	///         <see cref="InputStateExt" /> class contains a set of convenience methods for querying a range of devices
	///         on a range of axes.
	///     </para>
	/// </remarks>
	/// <threadsafety static="false" instance="false" />
	/// <seealso cref="InputStateExt" />
	[SuppressMessage("ReSharper", "UnusedMember.Global")]
	[SuppressMessage("ReSharper", "MemberCanBePrivate.Global")]
	[SuppressMessage("ReSharper", "EventNeverSubscribedTo.Global")]
	[SuppressMessage("ReSharper", "FieldCanBeMadeReadOnly.Global")]
	[SuppressMessage("ReSharper", "ConvertToConstant.Global")]
	[SuppressMessage("ReSharper", "UnassignedField.Global")]
	[SuppressMessage("ReSharper", "ClassNeverInstantiated.Global")]
	public sealed class InputState : MonoBehaviour
	{
		private static InputState _instance;

		private IntPtr _context;
		private Logger _log;
		private Native.LogCallback _logCallback;
		private Native.DeviceCallback _deviceCallback;
		private Dictionary<long, NativeDevice> _devices;
		private bool _ready;

		/// <summary>
		///     Preferred log level.
		/// </summary>
		public LogLevel LogLevel = LogLevel.Info;

		/// <summary>
		///     Switches to using FixedUpdate to drain events and poll for updates.
		/// </summary>
		public bool UseFixedUpdate;

		/// <summary>
		///     Toggles lifetime management. If <c>true</c>, <c>DontDestroyOnLoad</c> will be called on
		///     GameObject containing this component and extraneous instances will be automatically
		///     destroyed. <c>InputState</c> must strictly outlive all device objects, so this is
		///     necessary if you want <see cref="IDevice" /> objects to be usable after scene changes.
		/// </summary>
		public bool LifetimeManagement = true;

		/// <summary>
		///     The list of currently active devices. Subscribe to the <see cref="DeviceStateChanged" /> event
		///     to be notified when this list changes.
		/// </summary>
		/// <value>
		///     An <see cref="IEnumerable{T}"></see> of all known devices.
		///     It's not backed by a collection: you need to drain the elements with <see cref="Enumerable.ToList{TSource}" /> or
		///     similar
		///     if you want to iterate over it more than once.
		/// </value>
		/// <seealso cref="DeviceStateChanged" />
		/// <seealso cref="IDevice" />
		[NotNull]
		public IEnumerable<IDevice> Devices => _devices.Values.Cast<IDevice>();

		/// <summary>
		///     Event fired when the <see cref="Devices" /> list changes.
		/// </summary>
		public DeviceStateChangedEvent DeviceStateChanged = new DeviceStateChangedEvent();

		/// <summary>
		///     Event fired after the <see cref="Devices" /> list is first populated.
		///     Will only be fired once during the component lifetime (use <see cref="DeviceStateChanged" />
		///     event to get updates).
		/// </summary>
		public DevicesEnumeratedEvent DevicesEnumerated = new DevicesEnumeratedEvent();

		#region Unity callbacks

		[UsedImplicitly]
		private void Awake()
		{
			_log = new Logger(LogLevel);

			if ((_instance != null) && (_instance != this) && _instance.LifetimeManagement)
			{
				_log.Debug(
					"Lifetime management enabled, destroying this instance. " +
					"Remember that InputState instance survives scene changes.");
				DestroyImmediate(gameObject);
				return;
			}

			if (!LifetimeManagement)
			{
				return;
			}

			_log.Debug(@"Lifetime management enabled, calling DontDestroyOnLoad.");
			_instance = this;
			DontDestroyOnLoad(gameObject);
		}

		[UsedImplicitly]
		private void Start()
		{
			Input.ResetInputAxes();
			CreateContext();
		}

		[UsedImplicitly]
		private void Update()
		{
			if (UseFixedUpdate)
			{
				return;
			}

			DoUpdate();
		}

		[UsedImplicitly]
		private void FixedUpdate()
		{
			if (!UseFixedUpdate)
			{
				return;
			}

			DoUpdate();
		}

		[UsedImplicitly]
		private void OnDestroy()
		{
			_ready = false;
			Native.Destroy(_context);
		}

		#endregion Unity callbacks

		private void CreateContext()
		{
			_logCallback = _log.Callback;
			_deviceCallback = DeviceCallbackImpl;
			_devices = new Dictionary<long, NativeDevice>();

			var options = Native.CreateOptions();
			Native.SetLogLevel(options, LogLevel);
			Native.SetCustomLogSink(options, Marshal.GetFunctionPointerForDelegate(_logCallback), IntPtr.Zero);
			Native.SetDeviceCallback(options, Marshal.GetFunctionPointerForDelegate(_deviceCallback), IntPtr.Zero);

			_context = Native.Create(options);
			Native.DestroyOptions(options);

			if (_context == IntPtr.Zero)
			{
				_log.Error("Failed to create Multi-Input context.");
				enabled = false;
				return;
			}

			var @enum = Native.GetDevices(_context);
			var device = new Native.ApiDevice();
			while (Native.NextDevice(_context, @enum, ref device))
			{
				GetDevice(device.Id);
			}

			Native.DestroyEnumeration(_context, @enum);

			_ready = true;
			DevicesEnumerated?.Invoke();
		}

		private void DeviceCallbackImpl(IntPtr userData, DeviceEvent @event, long id, ref Native.ApiDevice info)
		{
			if (!_ready)
			{
				return;
			}

			IDevice device;

			// ReSharper disable once SwitchStatementMissingSomeCases
			if (@event == DeviceEvent.Removed)
			{
				try
				{
					device = _devices[id];
				}
				catch (KeyNotFoundException)
				{
					device = null;
				}

				_devices.Remove(id);
			}
			else
			{
				device = GetDevice(id);
			}

			DeviceStateChanged?.Invoke(device, @event);
		}

		private void DoUpdate()
		{
			Native.DrainEvents(_context);
		}

		[CanBeNull]
		internal NativeDevice GetDevice(long id)
		{
			// ReSharper disable once InvertIf
			if (!_devices.TryGetValue(id, out var device))
			{
				device = new NativeDevice(_log, _context, id);
				_devices.Add(id, device);
			}

			return device;
		}

		/// <summary>
		///     Resets the state of all axes on all devices.
		/// </summary>
		/// <remarks>
		///     This method works by committing a zero value on every supported axis
		///     on every known device.
		/// </remarks>
		public void Reset()
		{
			Native.Reset(_context);
		}

		/// <summary>
		///     Overload of
		///     <see
		///         cref="InputStateExt.FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;)" />
		///     that limits the search to selected axes, given as <see cref="IEnumerable&lt;T&gt;" /> argument.
		///     Use this form if you have a collection of input codes.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first
		///     <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" /> axis that
		///     reports a value of at least 0.5.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var axes = new List&lt;InputCode&gt; {
		///     InputCode.PadLeftTrigger,
		///     InputCode.PadRightTrigger
		/// };
		/// IDevice device;
		/// VirtualAxis axis;
		/// if (state.FindFirst(out device, out axis, v => v.Value >= 0.5f, axes)) {
		///     Debug.LogFormat("Found Value >= 0.5 on device {0}, axis {1}", device.Name, axis.Code);
		/// } else {
		///     Debug.LogFormat("Value >= 0.5 not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="foundDevice">
		///     If this method returned <c>true</c>, set to the found device. Otherwise,
		///     set to <c>null</c>.
		/// </param>
		/// <param name="foundAxis">
		///     If this method returned <c>true</c>, set to the found axis. Otherwise,
		///     set to <c>null</c>. You can check the axis code via <see cref="IVirtualAxis.Code" /> property.
		/// </param>
		/// <param name="predicate">
		///     Predicate for the search. Takes a <see cref="IDevice" /> object, an instance of <see cref="IVirtualAxis" />
		///     that belongs to that device, and should return <c>true</c> if said axis reports a value that interests you.
		/// </param>
		/// <param name="axes">Axes to search through.</param>
		/// <returns>
		///     <c>true</c> if predicate returned <c>true</c> for at least one axis, <c>false</c> otherwise.
		/// </returns>
		/// <seealso
		///     cref="InputStateExt.FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;)" />
		/// <seealso cref="InputStateExt.FindFirstDown(InputState, out IDevice, out IVirtualAxis, IEnumerable&lt;InputCode&gt;)" />
		/// <seealso cref="InputStateExt.FindFirstUp(InputState, out IDevice, out IVirtualAxis, IEnumerable&lt;InputCode&gt;)" />
		/// <seealso cref="InputStateExt.FindFirstHeld(InputState, out IDevice, out IVirtualAxis, IEnumerable&lt;InputCode&gt;)" />
		public bool FindFirst(
			[CanBeNull] out IDevice foundDevice,
			[CanBeNull] out IVirtualAxis foundAxis,
			Func<IDevice, IVirtualAxis, bool> predicate,
			[CanBeNull] IEnumerable<InputCode> axes)
		{
			foundAxis = null;
			foundDevice = null;

			if (!_ready)
			{
				return false;
			}

			var callback = new FindCallback(this, _context, predicate, axes);
			if (!callback.Run())
			{
				return false;
			}

			foundAxis = callback.FoundAxis;
			foundDevice = callback.FoundDevice;
			return true;
		}
	}
}
