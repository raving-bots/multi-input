// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

using System.Diagnostics.CodeAnalysis;

using UnityEngine;

namespace RavingBots.MultiInput
{
	/// <summary>
	///     Concrete class for custom virtual axes.
	/// </summary>
	[SuppressMessage("ReSharper", "MemberCanBePrivate.Global")]
	[SuppressMessage("ReSharper", "UnusedMember.Global")]
	[SuppressMessage("ReSharper", "NotAccessedField.Global")]
	public sealed class VirtualAxis : IVirtualAxis
	{
		private struct Frame
		{
			public float Value;
			public bool IsHeld => !Mathf.Approximately(Value, 0.0f);
		}

		/// <summary>
		///     <c>true</c> if <see cref="IsHeld" /> was <c>true</c> in the previous frame
		///     and isn't in this one.
		/// </summary>
		/// <value>If <c>true</c>, this axis has returned to the rest state in the current frame.</value>
		public bool IsUp => _previous.IsHeld && !_current.IsHeld;

		/// <summary>
		///     <c>true</c> if <see cref="IsHeld" /> was <c>false</c> in the previous frame
		///     but isn't in this one.
		/// </summary>
		/// <value>If <c>true</c>, this axis has left rest state in the current frame.</value>
		public bool IsDown => !_previous.IsHeld && _current.IsHeld;

		/// <summary>
		///     <c>true</c> if this axis is not in the rest state in the current frame
		///     (i.e. <see cref="Value" /> is not zero).
		/// </summary>
		public bool IsHeld => _current.IsHeld;

		/// <summary>
		///     <c>true</c> if value on this axis has changed from the previous frame.
		/// </summary>
		public bool HasChanged => !Mathf.Approximately(_current.Value, _previous.Value);

		/// <summary>
		///     Value on this axis in the current frame.
		/// </summary>
		public float Value => _current.Value;

		/// <summary>
		///     Value on this axis in the previous frame.
		/// </summary>
		public float PreviousValue => _previous.Value;

		/// <summary>
		///     Uncommitted value on this axis for the next frame. May change multiple times
		///     during a single frame until <see cref="Commit">committed</see>.
		/// </summary>
		public float NextValue => _next.Value;

		/// <summary>
		///     <see cref="InputCode" /> associated with this axis, if any. Informative only.
		/// </summary>
		public InputCode Code { get; }

		private Frame _previous;
		private Frame _current;
		private Frame _next;

		/// <summary>
		///     Constructs a new virtual axis, optionally setting <see cref="Code" /> property.
		/// </summary>
		/// <param name="code">Input code to set <see cref="Code" /> property to, or <c>null</c>.</param>
		public VirtualAxis(InputCode code = InputCode.None)
		{
			Code = code;
			_previous = new Frame();
			_current = new Frame();
			_next = new Frame();
		}

		/// <summary>
		///     Sets the next frame value.
		/// </summary>
		/// <remarks>
		///     <note type="important">
		///         Remember to <see cref="Commit" /> your values.
		///     </note>
		/// </remarks>
		/// <param name="value">Value to set.</param>
		public void Set(float value)
		{
			_next.Value = value;
		}

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
		public void Set(bool held)
		{
			var value = held ? 1.0f : 0.0f;
			Set(value);
		}

		/// <summary>
		///     Adds a value to the uncommitted next frame. Convenience method for axes
		///     that accumulate input in-between frames.
		/// </summary>
		/// <param name="value">Value to add.</param>
		public void Add(float value)
		{
			Set(_next.Value + value);
		}

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
		public void Commit()
		{
			_previous.Value = _current.Value;
			_current.Value = _next.Value;
		}
	}
}
