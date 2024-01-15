// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics.CodeAnalysis;
using System.Linq;

using JetBrains.Annotations;

namespace RavingBots.MultiInput
{
	/// <summary>
	///     Various convenience extension methods for <see cref="InputState" />.
	/// </summary>
	[SuppressMessage("ReSharper", "MemberCanBePrivate.Global")]
	[SuppressMessage("ReSharper", "UnusedMember.Global")]
	[SuppressMessage("ReSharper", "NotAccessedField.Global")]
	public static class InputStateExt
	{
		/// <summary>
		///     Collection containing all values from <see cref="InputCode" /> enumeration, provided for convenience.
		/// </summary>
		public static readonly ReadOnlyCollection<InputCode> AllAxes;

		static InputStateExt()
		{
			var codes = Enum.GetValues(typeof(InputCode));
			var axes = codes.Cast<InputCode>().ToList();
			AllAxes = axes.AsReadOnly();
		}

		#region FindFirst overloads

		/// <summary>
		///     Queries all axes on all devices, and returns first for which a given predicate
		///     returns <c>true</c>. If you don't require a custom predicate (i.e. you just want to check
		///     <see cref="IVirtualAxis.IsHeld" /> or similar), you should use simpler variants like
		///     <see cref="FindFirstHeld(InputState, out IDevice, out IVirtualAxis)" />.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first input axis that reports
		///     a value of at least 0.5.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// IDevice device;
		/// IVirtualAxis axis;
		/// if (state.FindFirst(out device, out axis, a => a.Value >= 0.5f)) {
		///     Debug.LogFormat("Found Value >= 0.5 on device {0}, axis {1}", device.Name, axis.Code);
		/// } else {
		///     Debug.LogFormat("Value >= 0.5 not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
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
		/// <returns>
		///     <c>true</c> if predicate returned <c>true</c> for at least one axis, <c>false</c> otherwise.
		/// </returns>
		/// <seealso cref="FindFirstDown(InputState, out IDevice, out IVirtualAxis)" />
		/// <seealso cref="FindFirstUp(InputState, out IDevice, out IVirtualAxis)" />
		/// <seealso cref="FindFirstHeld(InputState, out IDevice, out IVirtualAxis)" />
		public static bool FindFirst(
			this InputState state,
			out IDevice foundDevice,
			out IVirtualAxis foundAxis,
			Func<IDevice, IVirtualAxis, bool> predicate)
		{
			return state.FindFirst(out foundDevice, out foundAxis, predicate, null);
		}

		/// <summary>
		///     Overload of
		///     <see cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;)" /> that
		///     limits
		///     the search to selected axes, given as <c>params</c> argument. Use this form if you're specifying input codes
		///     manually.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first
		///     <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" /> axis that
		///     reports a value of at least 0.5.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// IDevice device;
		/// IVirtualAxis axis;
		/// if (state.FindFirst(out device, out axis, v => v.Value >= 0.5f, InputCode.PadLeftTrigger, InputCode.PadRightTrigger)) {
		///     Debug.LogFormat("Found Value >= 0.5 on device {0}, axis {1}", device.Name, axis.Code);
		/// } else {
		///     Debug.LogFormat("Value >= 0.5 not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
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
		/// <seealso cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;)" />
		/// <seealso cref="FindFirstDown(InputState, out IDevice, out IVirtualAxis, InputCode[])" />
		/// <seealso cref="FindFirstUp(InputState, out IDevice, out IVirtualAxis, InputCode[])" />
		/// <seealso cref="FindFirstHeld(InputState, out IDevice, out IVirtualAxis, InputCode[])" />
		public static bool FindFirst(
			this InputState state,
			out IDevice foundDevice,
			out IVirtualAxis foundAxis,
			Func<IDevice, IVirtualAxis, bool> predicate,
			params InputCode[] axes)
		{
			return state.FindFirst(out foundDevice, out foundAxis, predicate, axes);
		}

		/// <summary>
		///     Overload of
		///     <see cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;)" /> that
		///     only returns the device. Use this form if you only need the device object.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first device that reports
		///     a value of at least 0.5 on any axis.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var device = state.FindFirst(a => a.Value >= 0.5f);
		/// if (device != null) {
		///     Debug.LogFormat("Found Value >= 0.5 on device {0}", device.Name);
		/// } else {
		///     Debug.LogFormat("Value >= 0.5 not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="predicate">
		///     Predicate for the search. Takes a <see cref="IDevice" /> object, an instance of <see cref="IVirtualAxis" />
		///     that belongs to that device, and should return <c>true</c> if said axis reports a value that interests you.
		/// </param>
		/// <returns>Found device, or <c>null</c> if none were found.</returns>
		/// <seealso cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;)" />
		/// <seealso cref="FindFirstDown(InputState)" />
		/// <seealso cref="FindFirstUp(InputState)" />
		/// <seealso cref="FindFirstHeld(InputState)" />
		[CanBeNull]
		public static IDevice FindFirst(this InputState state, Func<IDevice, IVirtualAxis, bool> predicate)
		{
			return state.FindFirst(predicate, (IEnumerable<InputCode>)null);
		}

		/// <summary>
		///     Overload of
		///     <see cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;)" /> that
		///     only returns the device and limits the search to selected axes, given as <c>params</c> argument.
		///     Use this form if you only need the device object and you're specifying input codes manually.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first device that reports
		///     a value of at least 0.5 on <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" />
		///     axes.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var device = state.FindFirst(a => a.Value >= 0.5f, InputCode.PadLeftTrigger, InputCode.PadRightTrigger);
		/// if (device != null) {
		///     Debug.LogFormat("Found Value >= 0.5 on device {0}", device.Name);
		/// } else {
		///     Debug.LogFormat("Value >= 0.5 not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="predicate">
		///     Predicate for the search. Takes a <see cref="IDevice" /> object, an instance of <see cref="IVirtualAxis" />
		///     that belongs to that device, and should return <c>true</c> if said axis reports a value that interests you.
		/// </param>
		/// <param name="axes">Axes to search through.</param>
		/// <returns>Found device, or <c>null</c> if none were found.</returns>
		/// <seealso
		///     cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;, InputCode[])" />
		/// <seealso cref="FindFirstDown(InputState, InputCode[])" />
		/// <seealso cref="FindFirstUp(InputState, InputCode[])" />
		/// <seealso cref="FindFirstHeld(InputState, InputCode[])" />
		[CanBeNull]
		public static IDevice FindFirst(
			this InputState state,
			Func<IDevice, IVirtualAxis, bool> predicate,
			params InputCode[] axes)
		{
			return state.FindFirst(predicate, (IEnumerable<InputCode>)axes);
		}

		/// <summary>
		///     Overload of
		///     <see cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;)" /> that
		///     only returns the device and limits
		///     the search to selected axes, given as <see cref="IEnumerable&lt;T&gt;" /> argument.
		///     Use this form if you only need the device object and you have a collection of input codes.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first device that reports
		///     a value of at least 0.5 on <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" />
		///     axes.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var axes = new List&lt;InputCode&gt; {
		///     InputCode.PadLeftTrigger,
		///     InputCode.PadRightTrigger
		/// };
		/// var device = state.FindFirst(a => a.Value >= 0.5f, axes);
		/// if (device != null) {
		///     Debug.LogFormat("Found Value >= 0.5 on device {0}", device.Name);
		/// } else {
		///     Debug.LogFormat("Value >= 0.5 not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="predicate">
		///     Predicate for the search. Takes a <see cref="IDevice" /> object, an instance of <see cref="IVirtualAxis" />
		///     that belongs to that device, and should return <c>true</c> if said axis reports a value that interests you.
		/// </param>
		/// <param name="axes">Axes to search through.</param>
		/// <returns>Found device, or <c>null</c> if none were found.</returns>
		/// <seealso
		///     cref="InputState.FindFirst(out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;, IEnumerable&lt;InputCode&gt;)" />
		/// <seealso cref="FindFirstDown(InputState, IEnumerable&lt;InputCode&gt;)" />
		/// <seealso cref="FindFirstUp(InputState, IEnumerable&lt;InputCode&gt;)" />
		/// <seealso cref="FindFirstHeld(InputState, IEnumerable&lt;InputCode&gt;)" />
		[CanBeNull]
		public static IDevice FindFirst(
			this InputState state,
			Func<IDevice, IVirtualAxis, bool> predicate,
			[CanBeNull] IEnumerable<InputCode> axes)
		{
			state.FindFirst(out var foundDevice, out _, predicate, axes);
			return foundDevice;
		}

		#endregion

		#region FindFirstHeld overloads

		/// <summary>
		///     Queries all axes on all devices, and returns the first for which <see cref="IVirtualAxis.IsHeld" />
		///     is <c>true</c>.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first input axis that
		///     reports non-zero value.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// IDevice device;
		/// IVirtualAxis axis;
		/// if (state.FindFirstHeld(out device, out axis)) {
		///     Debug.LogFormat("Found non-zero value on device {0}, axis {1}", device.Name, axis.Code);
		/// } else {
		///     Debug.LogFormat("Non-zero value not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="foundDevice">
		///     If this method returned <c>true</c>, set to the found device. Otherwise,
		///     set to <c>null</c>.
		/// </param>
		/// <param name="foundAxis">
		///     If this method returned <c>true</c>, set to the found axis. Otherwise,
		///     set to <c>null</c>. You can check the axis code via <see cref="IVirtualAxis.Code" /> property.
		/// </param>
		/// <returns>
		///     <c>true</c> if <see cref="IVirtualAxis.IsHeld" /> was <c>true</c> for at least one axis, <c>false</c> otherwise.
		/// </returns>
		/// <seealso cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;)" />
		public static bool FindFirstHeld(this InputState state, out IDevice foundDevice, out IVirtualAxis foundAxis)
		{
			return state.FindFirstHeld(out foundDevice, out foundAxis, (IEnumerable<InputCode>)null);
		}

		/// <summary>
		///     Overload of <see cref="FindFirstHeld(InputState, out IDevice, out IVirtualAxis)" /> that limits
		///     the search to selected axes, given as <c>params</c> argument. Use this form if you're specifying input codes
		///     manually.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first held
		///     <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" /> axis.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// IDevice device;
		/// IVirtualAxis axis;
		/// if (state.FindFirstHeld(out device, out axis, InputCode.PadLeftTrigger, InputCode.PadRightTrigger)) {
		///     Debug.LogFormat("Found held trigger on device {0}, axis {1}", device.Name, axis.Code);
		/// } else {
		///     Debug.LogFormat("Held trigger not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="foundDevice">
		///     If this method returned <c>true</c>, set to the found device. Otherwise,
		///     set to <c>null</c>.
		/// </param>
		/// <param name="foundAxis">
		///     If this method returned <c>true</c>, set to the found axis. Otherwise,
		///     set to <c>null</c>. You can check the axis code via <see cref="IVirtualAxis.Code" /> property.
		/// </param>
		/// <param name="axes">Axes to search through.</param>
		/// <returns>
		///     <c>true</c> if <see cref="IVirtualAxis.IsHeld" /> was <c>true</c> for at least one axis, <c>false</c> otherwise.
		/// </returns>
		/// <seealso
		///     cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;, InputCode[])" />
		public static bool FindFirstHeld(
			this InputState state,
			out IDevice foundDevice,
			out IVirtualAxis foundAxis,
			params InputCode[] axes)
		{
			return state.FindFirstHeld(out foundDevice, out foundAxis, (IEnumerable<InputCode>)axes);
		}

		/// <summary>
		///     Overload of <see cref="FindFirstHeld(InputState, out IDevice, out IVirtualAxis)" /> that limits
		///     the search to selected axes, given as <see cref="IEnumerable&lt;T&gt;" /> argument.
		///     Use this form if you have a collection of input codes.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first held
		///     <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" /> axis.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var axes = new List&lt;InputCode&gt; {
		///     InputCode.PadLeftTrigger,
		///     InputCode.PadRightTrigger
		/// };
		/// IDevice device;
		/// IVirtualAxis axis;
		/// if (state.FindFirstHeld(out device, out axis, axes)) {
		///     Debug.LogFormat("Found held trigger on device {0}, axis {1}", device.Name, axis.Code);
		/// } else {
		///     Debug.LogFormat("Held trigger not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="foundDevice">
		///     If this method returned <c>true</c>, set to the found device. Otherwise,
		///     set to <c>null</c>.
		/// </param>
		/// <param name="foundAxis">
		///     If this method returned <c>true</c>, set to the found axis. Otherwise,
		///     set to <c>null</c>. You can check the axis code via <see cref="IVirtualAxis.Code" /> property.
		/// </param>
		/// <param name="axes">Axes to search through.</param>
		/// <returns>
		///     <c>true</c> if <see cref="IVirtualAxis.IsHeld" /> was <c>true</c> for at least one axis, <c>false</c> otherwise.
		/// </returns>
		/// <seealso
		///     cref="InputState.FindFirst(out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;, IEnumerable&lt;InputCode&gt;)" />
		public static bool FindFirstHeld(
			this InputState state,
			out IDevice foundDevice,
			out IVirtualAxis foundAxis,
			[CanBeNull] IEnumerable<InputCode> axes)
		{
			return state.FindFirst(out foundDevice, out foundAxis, (d, a) => a.IsHeld, axes);
		}

		/// <summary>
		///     Overload of <see cref="FindFirstHeld(InputState, out IDevice, out IVirtualAxis)" /> that
		///     only returns the device. Use this form if you only need the device object.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first input axis that
		///     reports non-zero value.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var device = state.FindFirstHeld();
		/// if (device != null) {
		///     Debug.LogFormat("Found non-zero axis on device {0}", device.Name);
		/// } else {
		///     Debug.LogFormat("Non-zero axis not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <returns>Found device, or <c>null</c>.</returns>
		/// <seealso cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;)" />
		[CanBeNull]
		public static IDevice FindFirstHeld(this InputState state)
		{
			return state.FindFirstHeld((IEnumerable<InputCode>)null);
		}

		/// <summary>
		///     Overload of <see cref="FindFirstHeld(InputState, out IDevice, out IVirtualAxis)" /> that
		///     only returns the device and limits the search to selected axes, given as <c>params</c> argument.
		///     Use this form if you only need the device object and you're specifying input codes manually.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first held
		///     <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" /> axis.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var device = state.FindFirstHeld(InputCode.PadLeftTrigger, InputCode.PadRightTrigger);
		/// if (device != null) {
		///     Debug.LogFormat("Found held trigger on device {0}", device.Name);
		/// } else {
		///     Debug.LogFormat("Held trigger not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="axes">Axes to search through.</param>
		/// <returns>Found device, or <c>null</c>.</returns>
		/// <seealso
		///     cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;, InputCode[])" />
		[CanBeNull]
		public static IDevice FindFirstHeld(this InputState state, params InputCode[] axes)
		{
			return state.FindFirstHeld((IEnumerable<InputCode>)axes);
		}

		/// <summary>
		///     Overload of <see cref="FindFirstHeld(InputState, out IDevice, out IVirtualAxis)" /> that
		///     only returns the device and limits
		///     the search to selected axes, given as <see cref="IEnumerable&lt;T&gt;" /> argument.
		///     Use this form if you only need the device object and you have a collection of input codes.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first held
		///     <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" /> axis.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var axes = new List&lt;InputCode&gt; {
		///     InputCode.PadLeftTrigger,
		///     InputCode.PadRightTrigger
		/// };
		/// var device = state.FindFirstHeld(axes);
		/// if (device != null) {
		///     Debug.LogFormat("Found held trigger on device {0}", device.Name);
		/// } else {
		///     Debug.LogFormat("Held trigger not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="axes">Axes to search through.</param>
		/// <returns>Found device, or <c>null</c>.</returns>
		/// <seealso
		///     cref="InputState.FindFirst(out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;, IEnumerable&lt;InputCode&gt;)" />
		[CanBeNull]
		public static IDevice FindFirstHeld(this InputState state, [CanBeNull] IEnumerable<InputCode> axes)
		{
			state.FindFirstHeld(out var foundDevice, out _, axes);
			return foundDevice;
		}

		#endregion

		#region FindFirstUp overloads

		/// <summary>
		///     Queries all axes on all devices, and returns the first for which <see cref="IVirtualAxis.IsUp" />
		///     is <c>true</c>.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first input axis that
		///     became non-zero in this frame.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// IDevice device;
		/// IVirtualAxis axis;
		/// if (state.FindFirstUp(out device, out axis)) {
		///     Debug.LogFormat("Found IsUp on device {0}, axis {1}", device.Name, axis.Code);
		/// } else {
		///     Debug.LogFormat("IsUp not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="foundDevice">
		///     If this method returned <c>true</c>, set to the found device. Otherwise,
		///     set to <c>null</c>.
		/// </param>
		/// <param name="foundAxis">
		///     If this method returned <c>true</c>, set to the found axis. Otherwise,
		///     set to <c>null</c>. You can check the axis code via <see cref="IVirtualAxis.Code" /> property.
		/// </param>
		/// <returns>
		///     <c>true</c> if <see cref="IVirtualAxis.IsUp" /> was <c>true</c> for at least one axis, <c>false</c> otherwise.
		/// </returns>
		/// <seealso cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;)" />
		public static bool FindFirstUp(this InputState state, out IDevice foundDevice, out IVirtualAxis foundAxis)
		{
			return state.FindFirstUp(out foundDevice, out foundAxis, (IEnumerable<InputCode>)null);
		}

		/// <summary>
		///     Overload of <see cref="FindFirstUp(InputState, out IDevice, out IVirtualAxis)" /> that limits
		///     the search to selected axes, given as <c>params</c> argument. Use this form if you're specifying input codes
		///     manually.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first
		///     <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" /> axis
		///     that became non-zero in this frame.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// IDevice device;
		/// IVirtualAxis axis;
		/// if (state.FindFirstUp(out device, out axis, InputCode.PadLeftTrigger, InputCode.PadRightTrigger)) {
		///     Debug.LogFormat("Found IsUp trigger on device {0}, axis {1}", device.Name, axis.Code);
		/// } else {
		///     Debug.LogFormat("IsUp trigger not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="foundDevice">
		///     If this method returned <c>true</c>, set to the found device. Otherwise,
		///     set to <c>null</c>.
		/// </param>
		/// <param name="foundAxis">
		///     If this method returned <c>true</c>, set to the found axis. Otherwise,
		///     set to <c>null</c>. You can check the axis code via <see cref="IVirtualAxis.Code" /> property.
		/// </param>
		/// <param name="axes">Axes to search through.</param>
		/// <returns>
		///     <c>true</c> if <see cref="IVirtualAxis.IsUp" /> was <c>true</c> for at least one axis, <c>false</c> otherwise.
		/// </returns>
		/// <seealso
		///     cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;, InputCode[])" />
		public static bool FindFirstUp(
			this InputState state,
			out IDevice foundDevice,
			out IVirtualAxis foundAxis,
			params InputCode[] axes)
		{
			return state.FindFirstUp(out foundDevice, out foundAxis, (IEnumerable<InputCode>)axes);
		}

		/// <summary>
		///     Overload of <see cref="FindFirstUp(InputState, out IDevice, out IVirtualAxis)" /> that limits
		///     the search to selected axes, given as <see cref="IEnumerable&lt;T&gt;" /> argument.
		///     Use this form if you have a collection of input codes.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first up
		///     <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" /> axis
		///     that became non-zero in this frame.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var axes = new List&lt;InputCode&gt; {
		///     InputCode.PadLeftTrigger,
		///     InputCode.PadRightTrigger
		/// };
		/// IDevice device;
		/// IVirtualAxis axis;
		/// if (state.FindFirstUp(out device, out axis, axes)) {
		///     Debug.LogFormat("Found IsUp trigger on device {0}, axis {1}", device.Name, axis.Code);
		/// } else {
		///     Debug.LogFormat("IsUp trigger not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="foundDevice">
		///     If this method returned <c>true</c>, set to the found device. Otherwise,
		///     set to <c>null</c>.
		/// </param>
		/// <param name="foundAxis">
		///     If this method returned <c>true</c>, set to the found axis. Otherwise,
		///     set to <c>null</c>. You can check the axis code via <see cref="IVirtualAxis.Code" /> property.
		/// </param>
		/// <param name="axes">Axes to search through.</param>
		/// <returns>
		///     <c>true</c> if <see cref="IVirtualAxis.IsUp" /> was <c>true</c> for at least one axis, <c>false</c> otherwise.
		/// </returns>
		/// <seealso
		///     cref="InputState.FindFirst(out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;, IEnumerable&lt;InputCode&gt;)" />
		public static bool FindFirstUp(
			this InputState state,
			out IDevice foundDevice,
			out IVirtualAxis foundAxis,
			[CanBeNull] IEnumerable<InputCode> axes)
		{
			return state.FindFirst(out foundDevice, out foundAxis, (d, a) => a.IsUp, axes);
		}

		/// <summary>
		///     Overload of <see cref="FindFirstUp(InputState, out IDevice, out IVirtualAxis)" /> that
		///     only returns the device. Use this form if you only need the device object.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first input axis
		///     that became non-zero in this frame.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var device = state.FindFirstUp();
		/// if (device != null) {
		///     Debug.LogFormat("Found IsUp on device {0}", device.Name);
		/// } else {
		///     Debug.LogFormat("IsUp not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <returns>Found device, or <c>null</c>.</returns>
		/// <seealso cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;)" />
		[CanBeNull]
		public static IDevice FindFirstUp(this InputState state)
		{
			return state.FindFirstUp((IEnumerable<InputCode>)null);
		}

		/// <summary>
		///     Overload of <see cref="FindFirstUp(InputState, out IDevice, out IVirtualAxis)" /> that
		///     only returns the device and limits the search to selected axes, given as <c>params</c> argument.
		///     Use this form if you only need the device object and you're specifying input codes manually.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first up
		///     <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" /> axis
		///     that became non-zero in this frame.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var device = state.FindFirstUp(InputCode.PadLeftTrigger, InputCode.PadRightTrigger);
		/// if (device != null) {
		///     Debug.LogFormat("Found IsUp trigger on device {0}", device.Name);
		/// } else {
		///     Debug.LogFormat("IsUp trigger not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="axes">Axes to search through.</param>
		/// <returns>Found device, or <c>null</c>.</returns>
		/// <seealso
		///     cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;, InputCode[])" />
		[CanBeNull]
		public static IDevice FindFirstUp(this InputState state, params InputCode[] axes)
		{
			return state.FindFirstUp((IEnumerable<InputCode>)axes);
		}

		/// <summary>
		///     Overload of <see cref="FindFirstUp(InputState, out IDevice, out IVirtualAxis)" /> that
		///     only returns the device and limits
		///     the search to selected axes, given as <see cref="IEnumerable&lt;T&gt;" /> argument.
		///     Use this form if you only need the device object and you have a collection of input codes.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first
		///     <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" /> axis
		///     that became non-zero in this frame.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var axes = new List&lt;InputCode&gt; {
		///     InputCode.PadLeftTrigger,
		///     InputCode.PadRightTrigger
		/// };
		/// var device = state.FindFirstUp(axes);
		/// if (device != null) {
		///     Debug.LogFormat("Found IsUp trigger on device {0}", device.Name);
		/// } else {
		///     Debug.LogFormat("IsUp trigger not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="axes">Axes to search through.</param>
		/// <returns>Found device, or <c>null</c>.</returns>
		/// <seealso
		///     cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;, InputCode[])" />
		[CanBeNull]
		public static IDevice FindFirstUp(this InputState state, [CanBeNull] IEnumerable<InputCode> axes)
		{
			state.FindFirstUp(out var foundDevice, out _, axes);
			return foundDevice;
		}

		#endregion

		#region FindFirstDown overloads

		/// <summary>
		///     Queries all axes on all devices, and returns the first for which <see cref="IVirtualAxis.IsDown" />
		///     is <c>true</c>.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first input axis
		///     that became zero in this frame.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// IDevice device;
		/// IVirtualAxis axis;
		/// if (state.FindFirstDown(out device, out axis)) {
		///     Debug.LogFormat("Found IsDown on device {0}, axis {1}", device.Name, axis.Code);
		/// } else {
		///     Debug.LogFormat("IsDown not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="foundDevice">
		///     If this method returned <c>true</c>, set to the found device. Otherwise,
		///     set to <c>null</c>.
		/// </param>
		/// <param name="foundAxis">
		///     If this method returned <c>true</c>, set to the found axis. Otherwise,
		///     set to <c>null</c>. You can check the axis code via <see cref="IVirtualAxis.Code" /> property.
		/// </param>
		/// <returns>
		///     <c>true</c> if <see cref="IVirtualAxis.IsDown" /> was <c>true</c> for at least one axis, <c>false</c> otherwise.
		/// </returns>
		/// <seealso cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;)" />
		public static bool FindFirstDown(this InputState state, out IDevice foundDevice, out IVirtualAxis foundAxis)
		{
			return state.FindFirstDown(out foundDevice, out foundAxis, (IEnumerable<InputCode>)null);
		}

		/// <summary>
		///     Overload of <see cref="FindFirstDown(InputState, out IDevice, out IVirtualAxis)" /> that limits
		///     the search to selected axes, given as <c>params</c> argument. Use this form if you're specifying input codes
		///     manually.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first
		///     <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" /> axis
		///     that became zero in this frame.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// IDevice device;
		/// IVirtualAxis axis;
		/// if (state.FindFirstDown(out device, out axis, InputCode.PadLeftTrigger, InputCode.PadRightTrigger)) {
		///     Debug.LogFormat("Found IsDown trigger on device {0}, axis {1}", device.Name, axis.Code);
		/// } else {
		///     Debug.LogFormat("IsDown trigger not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="foundDevice">
		///     If this method returned <c>true</c>, set to the found device. Otherwise,
		///     set to <c>null</c>.
		/// </param>
		/// <param name="foundAxis">
		///     If this method returned <c>true</c>, set to the found axis. Otherwise,
		///     set to <c>null</c>. You can check the axis code via <see cref="IVirtualAxis.Code" /> property.
		/// </param>
		/// <param name="axes">Axes to search through.</param>
		/// <returns>
		///     <c>true</c> if <see cref="IVirtualAxis.IsDown" /> was <c>true</c> for at least one axis, <c>false</c> otherwise.
		/// </returns>
		/// <seealso
		///     cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;, InputCode[])" />
		public static bool FindFirstDown(
			this InputState state,
			out IDevice foundDevice,
			out IVirtualAxis foundAxis,
			params InputCode[] axes)
		{
			return state.FindFirstDown(out foundDevice, out foundAxis, (IEnumerable<InputCode>)axes);
		}

		/// <summary>
		///     Overload of <see cref="FindFirstDown(InputState, out IDevice, out IVirtualAxis)" /> that limits
		///     the search to selected axes, given as <see cref="IEnumerable&lt;T&gt;" /> argument.
		///     Use this form if you have a collection of input codes.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first
		///     <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" /> axis
		///     that became zero in this frame.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var axes = new List&lt;InputCode&gt; {
		///     InputCode.PadLeftTrigger,
		///     InputCode.PadRightTrigger
		/// };
		/// IDevice device;
		/// IVirtualAxis axis;
		/// if (state.FindFirstDown(out device, out axis, axes)) {
		///     Debug.LogFormat("Found IsDown trigger on device {0}, axis {1}", device.Name, axis.Code);
		/// } else {
		///     Debug.LogFormat("IsDown trigger not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="foundDevice">
		///     If this method returned <c>true</c>, set to the found device. Otherwise,
		///     set to <c>null</c>.
		/// </param>
		/// <param name="foundAxis">
		///     If this method returned <c>true</c>, set to the found axis. Otherwise,
		///     set to <c>null</c>. You can check the axis code via <see cref="IVirtualAxis.Code" /> property.
		/// </param>
		/// <param name="axes">Axes to search through.</param>
		/// <returns>
		///     <c>true</c> if <see cref="IVirtualAxis.IsDown" /> was <c>true</c> for at least one axis, <c>false</c> otherwise.
		/// </returns>
		/// <seealso
		///     cref="InputState.FindFirst(out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;, IEnumerable&lt;InputCode&gt;)" />
		public static bool FindFirstDown(
			this InputState state,
			out IDevice foundDevice,
			out IVirtualAxis foundAxis,
			[CanBeNull] IEnumerable<InputCode> axes)
		{
			return state.FindFirst(out foundDevice, out foundAxis, (d, a) => a.IsDown, axes);
		}

		/// <summary>
		///     Overload of <see cref="FindFirstDown(InputState, out IDevice, out IVirtualAxis)" /> that
		///     only returns the device. Use this form if you only need the device object.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first input axis
		///     that became zero in this frame.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var device = state.FindFirstDown();
		/// if (device != null) {
		///     Debug.LogFormat("Found IsDown on device {0}", device.Name);
		/// } else {
		///     Debug.LogFormat("IsDown not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <returns>Found device, or <c>null</c>.</returns>
		/// <seealso cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;)" />
		[CanBeNull]
		public static IDevice FindFirstDown(this InputState state)
		{
			return state.FindFirstDown((IEnumerable<InputCode>)null);
		}

		/// <summary>
		///     Overload of <see cref="FindFirstDown(InputState, out IDevice, out IVirtualAxis)" /> that
		///     only returns the device and limits the search to selected axes, given as <c>params</c> argument.
		///     Use this form if you only need the device object and you're specifying input codes manually.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first
		///     <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" /> axis
		///     that became zero in this frame.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var device = state.FindFirstDown(InputCode.PadLeftTrigger, InputCode.PadRightTrigger);
		/// if (device != null) {
		///     Debug.LogFormat("Found IsDown trigger on device {0}", device.Name);
		/// } else {
		///     Debug.LogFormat("IsDown trigger not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="axes">Axes to search through.</param>
		/// <returns>Found device, or <c>null</c>.</returns>
		/// <seealso
		///     cref="FindFirst(InputState, out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;, InputCode[])" />
		[CanBeNull]
		public static IDevice FindFirstDown(this InputState state, params InputCode[] axes)
		{
			return state.FindFirstDown((IEnumerable<InputCode>)axes);
		}

		/// <summary>
		///     Overload of <see cref="FindFirstDown(InputState, out IDevice, out IVirtualAxis)" /> that
		///     only returns the device and limits
		///     the search to selected axes, given as <see cref="IEnumerable&lt;T&gt;" /> argument.
		///     Use this form if you only need the device object and you have a collection of input codes.
		/// </summary>
		/// <example>
		///     The following example shows how you can use this method to find first
		///     <see cref="InputCode.PadLeftTrigger" /> or <see cref="InputCode.PadRightTrigger" /> axis
		///     that became zero in this frame.
		///     <code>
		/// var state = FindObjectOfType&lt;InputState&gt;();
		/// var axes = new List&lt;InputCode&gt; {
		///     InputCode.PadLeftTrigger,
		///     InputCode.PadRightTrigger
		/// };
		/// var device = state.FindFirstDown(axes);
		/// if (device != null) {
		///     Debug.LogFormat("Found IsDown trigger on device {0}", device.Name);
		/// } else {
		///     Debug.LogFormat("IsDown trigger not found");
		/// }
		/// </code>
		/// </example>
		/// <param name="state"><see cref="InputState" /> instance.</param>
		/// <param name="axes">Axes to search through.</param>
		/// <returns>Found device, or <c>null</c>.</returns>
		/// <seealso
		///     cref="InputState.FindFirst(out IDevice, out IVirtualAxis, Func&lt;IDevice, IVirtualAxis, bool&gt;, IEnumerable&lt;InputCode&gt;)" />
		[CanBeNull]
		public static IDevice FindFirstDown(this InputState state, [CanBeNull] IEnumerable<InputCode> axes)
		{
			state.FindFirstDown(out var foundDevice, out _, axes);
			return foundDevice;
		}

		#endregion
	}
}
