// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

using System.Diagnostics.CodeAnalysis;

namespace RavingBots.MultiInput
{
	/// <summary>
	///     Scalar values representing a virtual axis in previous, current and
	///     next frame.
	/// </summary>
	/// <remarks>
	///     <para>
	///         This interface is one of the two core abstractions in Multi-Input (the other one
	///         being <see cref="IDevice" />). All of your input queries will return objects that
	///         implement <c>IVirtualAxis</c>.
	///     </para>
	///     <para>
	///         Virtual axes track their value in a sliding window of previous, current and next frame.
	///         Value of the next frame is fetched from platform-specific sources and then <see cref="Commit">committed</see>
	///         at
	///         the beginning of the frame. Committing replaces previous value with current value, and current value with next
	///         value.
	///     </para>
	///     <para>
	///         All supported inputs (be it digital buttons, analog axes, or completely virtual axes that have no direct
	///         physical
	///         equivalent) are represented as a scalar floating point value. The exact range depends on the axis in question
	///         (for example, buttons will only report discrete 0 or 1, while analog axes will report contiguous values in
	///         various
	///         ranges, e.g. <c>[0, 1]</c> or <c>[-1, 1]</c>).
	///     </para>
	///     <note type="important">
	///         You can (and likely will) create your own virtual axes that aren't attached to any devices (see
	///         <see cref="VirtualAxis" />).
	///         Remember to <see cref="Commit" /> all the values you <see cref="Set(float)" /> on these axes.
	///     </note>
	/// </remarks>
	/// <seealso cref="VirtualAxis" />
	/// <threadsafety static="false" instance="false" />
	[SuppressMessage("ReSharper", "UnusedMember.Global")]
	[SuppressMessage("ReSharper", "UnusedMemberInSuper.Global")]
	public interface IVirtualAxis
	{
		/// <summary>
		///     <c>true</c> if <see cref="IsHeld" /> was <c>true</c> in the previous frame
		///     and isn't in this one.
		/// </summary>
		/// <value>If <c>true</c>, this axis has returned to the rest state in the current frame.</value>
		bool IsUp { get; }

		/// <summary>
		///     <c>true</c> if <see cref="IsHeld" /> was <c>false</c> in the previous frame
		///     but isn't in this one.
		/// </summary>
		/// <value>If <c>true</c>, this axis has left rest state in the current frame.</value>
		bool IsDown { get; }

		/// <summary>
		///     <c>true</c> if this axis is not in the rest state in the current frame
		///     (i.e. <see cref="Value" /> is not zero).
		/// </summary>
		bool IsHeld { get; }

		/// <summary>
		///     <c>true</c> if value on this axis has changed from the previous frame.
		/// </summary>
		bool HasChanged { get; }

		/// <summary>
		///     Value on this axis in the current frame.
		/// </summary>
		float Value { get; }

		/// <summary>
		///     Value on this axis in the previous frame.
		/// </summary>
		float PreviousValue { get; }

		/// <summary>
		///     Uncommitted value on this axis for the next frame. May change multiple times
		///     during a single frame until <see cref="Commit">committed</see>.
		/// </summary>
		float NextValue { get; }

		/// <summary>
		///     Input code associated with this of this axis or <see cref="InputCode.None" />.
		/// </summary>
		InputCode Code { get; }

		/// <summary>
		///     Sets the next frame value.
		/// </summary>
		/// <remarks>
		///     <note type="important">
		///         Remember to <see cref="Commit" /> your values.
		///     </note>
		/// </remarks>
		/// <param name="value">Value to set.</param>
		void Set(float value);

		/// <summary>
		///     Sets the next frame value. Convenience method for axes representing
		///     digital buttons.
		/// </summary>
		/// <remarks>
		///     <note type="important">
		///         Remember to <see cref="Commit" /> your values.
		///     </note>
		/// </remarks>
		/// <param name="held">
		///     If <c>true</c>, will <see cref="Set(float)" /> the axis value to 1. Otherwise,
		///     will set the value to 0.
		/// </param>
		void Set(bool held);

		/// <summary>
		///     Adds a value to the uncommitted next frame. Convenience method for axes
		///     that accumulate input in-between frames.
		/// </summary>
		/// <param name="value">Value to add.</param>
		void Add(float value);

		/// <summary>
		///     Commits changes to the next frame.
		/// </summary>
		/// <remarks>
		///     After this method is called, both previous and current values will change.
		///     <note type="important">
		///         This <b>does not</b> clear the uncommitted value, as some axes need to report
		///         the same value until they're explicitly reset.
		///     </note>
		/// </remarks>
		void Commit();
	}
}
