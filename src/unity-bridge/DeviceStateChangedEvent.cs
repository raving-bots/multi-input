// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

using System;

using UnityEngine.Events;

namespace RavingBots.MultiInput
{
	/// <summary>
	///     Event type for the <see cref="InputState.DeviceStateChanged" /> event.
	///     You will get a device object and the enumerator describing the change.
	///     The device object may be <c>null</c> for removed devices.
	/// </summary>
	[Serializable]
	public class DeviceStateChangedEvent : UnityEvent<IDevice, DeviceEvent>
	{
	}
}
