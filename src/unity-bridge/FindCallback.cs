// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

using JetBrains.Annotations;

namespace RavingBots.MultiInput
{
	internal sealed class FindCallback
	{
		private readonly InputState _state;
		private readonly IntPtr _context;
		private readonly Func<IDevice, IVirtualAxis, bool> _predicate;
		private readonly InputCode[] _axes;
		private readonly Native.FindCallback _callback;

		// ReSharper disable once ConvertToAutoProperty
		public IVirtualAxis FoundAxis { get; private set; }

		// ReSharper disable once ConvertToAutoProperty
		public IDevice FoundDevice { get; private set; }

		internal FindCallback(
			InputState state,
			IntPtr context,
			[NotNull] Func<IDevice, IVirtualAxis, bool> predicate,
			[CanBeNull] IEnumerable<InputCode> axes)
		{
			_state = state;
			_context = context;
			_predicate = predicate;
			_axes = axes?.ToArray();
			_callback = Callback;
		}

		internal bool Run()
		{
			var callback = Marshal.GetFunctionPointerForDelegate(_callback);
			var id = -1L;
			var code = InputCode.None;
			var size = (uint?)_axes?.Length ?? 0U;

			if (!Native.FindFirst(_context, callback, IntPtr.Zero, _axes, size, ref id, ref code))
			{
				return false;
			}

			var device = _state.GetDevice(id);
			var axis = device?[code];

			if (axis == null)
			{
				return false;
			}

			FoundDevice = device;
			FoundAxis = axis;
			return true;
		}

		private int Callback(IntPtr userData, long id, InputCode code, float current, float previous, float next)
		{
			var device = _state.GetDevice(id);
			var axis = device?[code];

			if (axis == null)
			{
				return 0;
			}

			return _predicate(device, axis) ? 1 : 0;
		}
	}
}
