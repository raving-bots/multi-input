// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

using System;

using UnityEngine;

namespace RavingBots.MultiInput
{
	internal sealed class NativeVirtualAxis : IVirtualAxis
	{
		private readonly IntPtr _context;
		private readonly long _device;

		private bool PreviousHeld => !Mathf.Approximately(PreviousValue, 0);
		public bool IsHeld => !Mathf.Approximately(Value, 0);

		public bool IsUp => !IsHeld && PreviousHeld;
		public bool IsDown => IsHeld && !PreviousHeld;
		public bool HasChanged => !Mathf.Approximately(Value, PreviousValue);

		public float Value => Native.GetValue(_context, _device, Code);
		public float PreviousValue => Native.GetPrevious(_context, _device, Code);
		public float NextValue => Native.GetNext(_context, _device, Code);
		public InputCode Code { get; }

		internal NativeVirtualAxis(IntPtr context, long device, InputCode code)
		{
			Code = code;
			_device = device;
			_context = context;
		}

		public void Set(float value)
		{
			Native.SetValue(_context, _device, Code, value);
		}

		public void Set(bool held)
		{
			Set(held ? 1.0f : 0.0f);
		}

		public void Add(float value)
		{
			Native.AddValue(_context, _device, Code, value);
		}

		public void Commit()
		{
			Native.CommitValue(_context, _device, Code);
		}
	}
}
