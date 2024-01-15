// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;

namespace RavingBots.MultiInput
{
	/// <summary>
	///     Uniform interface to all device objects.
	/// </summary>
	/// <remarks>
	///     This interface is one of the two core abstractions in Multi-Input (the other one
	///     being <see cref="IVirtualAxis" />). Most of your interaction with this library will
	///     be through device objects and this interface.
	///     <note type="note">
	///         We plan to make comprehensive API for extending the library with your own devices,
	///         but all of the internal machinery is still in flux and might change as we expand the
	///         device and platform support.
	///     </note>
	/// </remarks>
	/// <threadsafety static="false" instance="false" />
	[SuppressMessage("ReSharper", "UnusedMemberInSuper.Global")]
	[SuppressMessage("ReSharper", "UnusedMember.Global")]
	public interface IDevice
	{
		/// <summary>
		///     A session-unique ID of this device.
		/// </summary>
		/// <remarks>
		///     <para>
		///         This ID is not persistent. You must not store it, or
		///         expect any particular device to always have the same ID.
		///     </para>
		/// </remarks>
		long Id { get; }

		/// <summary>
		///     Friendly name of this device.
		/// </summary>
		/// <remarks>
		///     <para>
		///         Might be empty, but will never be null.
		///     </para>
		///     <note type="important">
		///         This value can only be used if <see cref="IsUsable" /> is <c>true</c>.
		///     </note>
		/// </remarks>
		string Name { get; }

		/// <summary>
		///     Value describing the bus location of the device.
		/// </summary>
		/// <remarks>
		///     <para>
		///         Might be empty, but will never be null.
		///     </para>
		///     <para>
		///         Can be used to try to determine (on best-effort basis) whether device
		///         that just reconnected is the same one the player has been using.
		///     </para>
		///     <para>
		///         However, it's NOT guaranteed to be useful, unique or persistent.
		///         It highly depends on the OS, the driver and the device. It's very likely
		///         that you will have multiple devices sharing the same Location value. Make
		///         sure your code can handle that.
		///     </para>
		///     <para>
		///         Don't try to parse or display this value. The only meaningful thing you can do with
		///         it is to compare it with a Location property of another device.
		///     </para>
		///     <note type="important">
		///         This value can only be used if <see cref="IsUsable" /> is <c>true</c>.
		///     </note>
		/// </remarks>
		string Location { get; }

		/// <summary>
		///     Value containing internal name/ID of the device reported by the OS.
		/// </summary>
		/// <remarks>
		///     <para>Might be empty, but will never be null.</para>
		///     <para>
		///         Don't try to parse or display this value. The only meaningful thing you can do with
		///         it is to compare it with a InternalName property of another device.
		///     </para>
		///     <note type="important">
		///         This value can only be used if <see cref="IsUsable" /> is <c>true</c>.
		///     </note>
		/// </remarks>
		string InternalName { get; }

		/// <summary>
		///     Vendor ID reported by the device/driver.
		/// </summary>
		/// <remarks>
		///     <para>Might be zero.</para>
		///     <note type="important">
		///         This value can only be used if <see cref="IsUsable" /> is <c>true</c>.
		///     </note>
		/// </remarks>
		int VendorId { get; }

		/// <summary>
		///     Product ID reported by the device/driver.
		/// </summary>
		/// <remarks>
		///     <para>Might be zero.</para>
		///     <note type="important">
		///         This value can only be used if <see cref="IsUsable" /> is <c>true</c>.
		///     </note>
		/// </remarks>
		int ProductId { get; }

		/// <summary>
		///     Revision/version reported by the device/driver.
		/// </summary>
		/// <remarks>
		///     <para>Might be zero.</para>
		///     <note type="important">
		///         This value can only be used if <see cref="IsUsable" /> is <c>true</c>.
		///     </note>
		/// </remarks>
		int Revision { get; }

		/// <summary>
		///     Serial number reported by the device/driver.
		/// </summary>
		/// <remarks>
		///     <para>Might be empty, but will never be null.</para>
		/// </remarks>
		string Serial { get; }

		/// <summary>
		///     <c>true</c> if device is usable. The exact meaning of this is device-specific,
		///     but unusable devices should not be considered when checking for input.
		/// </summary>
		bool IsUsable { get; }

		/// <summary>
		///     <c>true</c> if device supports vibration.
		/// </summary>
		/// <remarks>
		///     If this is <c>false</c>, <see cref="Vibrate" /> is a no-op and always returns <c>false</c>.
		/// </remarks>
		bool CanVibrate { get; }

		/// <summary>
		///     Returns a virtual axis object or <c>null</c> if the axis is not supported by this device.
		/// </summary>
		/// <remarks>
		///     Devices are allowed to claim support for axes but never report any values on them.
		/// </remarks>
		/// <param name="code">Code of the axis.</param>
		IVirtualAxis this[InputCode code] { get; }

		/// <summary>
		///     Enumerable of all axes this device supports.
		/// </summary>
		IEnumerable<InputCode> SupportedAxes { get; }

		/// <preliminary>
		///     This method is currently designed for XBox-compatible controllers. The vibration API might still change
		///     as we expand the device and platform support.
		/// </preliminary>
		/// <summary>
		///     Makes this device vibrate for specified amount of time.
		/// </summary>
		/// <param name="duration">Duration of the effect, in milliseconds.</param>
		/// <param name="left">Strength of the left motor vibration, in range <c>[0, 1]</c>. 0 immediately turns the motor off.</param>
		/// <param name="right">Strength of the right motor vibration, in range <c>[0, 1]</c>. 0 immediately turns the motor off.</param>
		/// <returns>
		///     <c>true</c> if the signal was sent successfully to the device
		///     (not necessarily that the device acted on it).
		/// </returns>
		bool Vibrate(int duration, float left, float right);

		/// <summary>
		///     Resets the state of all axes on this device.
		/// </summary>
		void Reset();
	}
}
