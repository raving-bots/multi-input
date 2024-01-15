// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.InteropServices;
using System.Text;

using JetBrains.Annotations;

namespace RavingBots.MultiInput
{
	[SuppressMessage("ReSharper", "ClassNeverInstantiated.Global")]
	public static class Native
	{
		private const string Library = "rb-minput";

		[StructLayout(LayoutKind.Sequential)]
		[SuppressMessage("ReSharper", "FieldCanBeMadeReadOnly.Global")]
		[SuppressMessage("ReSharper", "MemberHidesStaticFromOuterClass")]
		[SuppressMessage("ReSharper", "MemberCanBePrivate.Global")]
		public struct ApiDevice
		{
			public long Id;

			public IntPtr Name;
			public IntPtr LocationId;
			public IntPtr publicId;
			public int VendorId;
			public int ProductId;
			public int Revision;
			public IntPtr Serial;

			public int IsUsable;
			public int CanVibrate;
			public uint AxisCount;
		}

		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		public delegate void LogCallback(IntPtr userData, LogLevel level, IntPtr message);

		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		public delegate int FindCallback(
			IntPtr userData,
			long id,
			InputCode code,
			float current,
			float previous,
			float next);

		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		public delegate void DeviceCallback(IntPtr userData, DeviceEvent @event, long id, ref ApiDevice device);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_create_options")]
		public static extern IntPtr CreateOptions();

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_set_log_level")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool SetLogLevel(IntPtr options, LogLevel level);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_set_custom_log_sink")
		]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool SetCustomLogSink(
			IntPtr options,
			[MarshalAs(UnmanagedType.FunctionPtr)] IntPtr callback,
			IntPtr userData);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_set_device_callback")
		]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool SetDeviceCallback(
			IntPtr options,
			[MarshalAs(UnmanagedType.FunctionPtr)] IntPtr callback,
			IntPtr userData);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_destroy_options")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool DestroyOptions(IntPtr options);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_create")]
		public static extern IntPtr Create(IntPtr options);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_destroy")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool Destroy(IntPtr context);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_reset")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool Reset(IntPtr context);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_drain_events")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool DrainEvents(IntPtr context);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_get_devices")]
		public static extern IntPtr GetDevices(IntPtr context);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_next_device")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool NextDevice(IntPtr context, IntPtr @enum, ref ApiDevice device);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_destroy_enumeration")
		]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool DestroyEnumeration(IntPtr context, IntPtr @enum);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_get_device")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool GetDevice(
			IntPtr context,
			[MarshalAs(UnmanagedType.I8)] long id,
			ref ApiDevice device);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_is_usable")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool IsUsable(
			IntPtr context,
			[MarshalAs(UnmanagedType.I8)] long id);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_can_vibrate")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool CanVibrate(
			IntPtr context,
			[MarshalAs(UnmanagedType.I8)] long id);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_vibrate")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool Vibrate(
			IntPtr context,
			[MarshalAs(UnmanagedType.I8)] long id,
			int duration,
			float left,
			float right);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_reset_device")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool ResetDevice(
			IntPtr context,
			[MarshalAs(UnmanagedType.I8)] long id);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_get_axis_count")]
		[return: MarshalAs(UnmanagedType.SysUInt)]
		public static extern uint GetAxisCount(
			IntPtr context,
			[MarshalAs(UnmanagedType.I8)] long id);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_get_axes")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool GetAxes(
			IntPtr context,
			long id,
			[MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)]
			InputCode[] buffer,
			[MarshalAs(UnmanagedType.SysUInt)] uint size);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_has_axis")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool HasAxis(
			IntPtr context,
			long id,
			InputCode code);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_get_value")]
		public static extern float GetValue(
			IntPtr context,
			[MarshalAs(UnmanagedType.I8)] long id,
			InputCode code);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_get_previous")]
		public static extern float GetPrevious(
			IntPtr context,
			[MarshalAs(UnmanagedType.I8)] long id,
			InputCode code);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_get_next")]
		public static extern float GetNext(
			IntPtr context,
			[MarshalAs(UnmanagedType.I8)] long id,
			InputCode code);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_set_value")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool SetValue(
			IntPtr context,
			[MarshalAs(UnmanagedType.I8)] long id,
			InputCode code,
			float next);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_add_value")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool AddValue(
			IntPtr context,
			[MarshalAs(UnmanagedType.I8)] long id,
			InputCode code,
			float next);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_commit_value")]
		public static extern bool CommitValue(
			IntPtr context,
			[MarshalAs(UnmanagedType.I8)] long id,
			InputCode code);

		[DllImport(Library, CallingConvention = CallingConvention.StdCall, EntryPoint = "rb_minput_find_first")]
		[return: MarshalAs(UnmanagedType.Bool)]
		public static extern bool FindFirst(
			IntPtr context,
			[MarshalAs(UnmanagedType.FunctionPtr)] IntPtr callback,
			IntPtr userData,
			[CanBeNull] [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4)]
			InputCode[] codes,
			[MarshalAs(UnmanagedType.SysUInt)] uint codesSize,
			[MarshalAs(UnmanagedType.I8)] ref long outId,
			ref InputCode outCode);

		public static string Decode(IntPtr str)
		{
			if (str == IntPtr.Zero)
			{
				return "";
			}

			var buffer = new List<byte>();

			var offset = 0;
			while (true)
			{
				var x = Marshal.ReadByte(str, offset++);
				if (x == 0)
				{
					break;
				}

				buffer.Add(x);
			}

			if (buffer.Count == 0)
			{
				return "";
			}

			return Encoding.UTF8.GetString(buffer.ToArray());
		}
	}
}
