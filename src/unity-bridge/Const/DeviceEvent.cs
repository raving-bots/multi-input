// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

using System.Diagnostics.CodeAnalysis;

namespace RavingBots.MultiInput
{
	/// <summary>
	///     Enumeration for device state events.
	/// </summary>
	/// <seealso cref="InputState.DeviceStateChanged" />
	[SuppressMessage("ReSharper", "UnusedMember.Global")]
	public enum DeviceEvent
	{
		// TODO sync with device_event.hpp
		/// <summary>
		///     The device object has been created (i.e. a new device has been detected).
		/// </summary>
		Created = 1,

		/// <summary>
		///     The device object has been removed (i.e. the device is no longer present in the system).
		///     You should discard the affected device object, it will no longer be present on the
		///     <see cref="InputState.Devices" /> list.
		/// </summary>
		Removed = 2,

		/// <summary>
		///     The device has been marked unusable and will no longer produce usable input.
		/// </summary>
		Unusable = 3,

		/// <summary>
		///     The device has been marked usable and will produce usable input again.
		/// </summary>
		Usable = 4
	}
}
