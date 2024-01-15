// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

using System.Diagnostics.CodeAnalysis;

namespace RavingBots.MultiInput
{
	/// <summary>
	///     Several utility methods and constants for <see cref="InputCode" />.
	/// </summary>
	/// <seealso cref="InputCode" />
	/// <threadsafety static="true" instance="true" />
	[SuppressMessage("ReSharper", "UnusedMember.Global")]
	[SuppressMessage("ReSharper", "MemberCanBePrivate.Global")]
	public static class InputCodeExt
	{
		/// <summary>
		///     The start of the keyboard code range. The range extends to <see cref="KeyboardRangeEnd" /> (inclusive).
		/// </summary>
		public const int KeyboardRangeStart = 100000;

		/// <summary>
		///     The end of the keyboard code range (inclusive).
		/// </summary>
		public const int KeyboardRangeEnd = 199999;

		/// <summary>
		///     The start of reserved subrange of keyboard codes. This is the first value you can use for your custom codes.
		/// </summary>
		public const int KeyboardCustomRangeStart = 150000;

		/// <summary>
		///     The start of the mouse code range. The range extends to <see cref="MouseRangeEnd" /> (inclusive).
		/// </summary>
		public const int MouseRangeStart = 200000;

		/// <summary>
		///     The end of the mouse code range (inclusive).
		/// </summary>
		public const int MouseRangeEnd = 299999;

		/// <summary>
		///     The start of reserved subrange of mouse codes. This is the first value you can use for your custom codes.
		/// </summary>
		public const int MouseCustomRangeStart = 250000;

		/// <summary>
		///     The start of the gamepad code range. The range extends to <see cref="PadRangeEnd" /> (inclusive).
		/// </summary>
		public const int PadRangeStart = 300000;

		/// <summary>
		///     The end of the gamepad code range (inclusive).
		/// </summary>
		public const int PadRangeEnd = 399999;

		/// <summary>
		///     The start of reserved subrange of gamepad codes. This is the first value you can use for your custom codes.
		/// </summary>
		public const int PadCustomRangeStart = 350000;

		/// <summary>
		///     Returns <c>true</c> if given <see cref="InputCode" /> is a keyboard code.
		/// </summary>
		/// <param name="code">The code to check.</param>
		/// <returns>Whether the code is a keyboard code.</returns>
		/// <seealso cref="KeyboardRangeStart" />
		/// <seealso cref="KeyboardRangeEnd" />
		public static bool IsKeyboard(this InputCode code)
		{
			var value = (int)code;
			return (value >= KeyboardRangeStart) && (value <= KeyboardRangeEnd);
		}

		/// <summary>
		///     Returns <c>true</c> if given <see cref="InputCode" /> is a mouse code.
		/// </summary>
		/// <param name="code">The code to check.</param>
		/// <returns>Whether the code is a mouse code.</returns>
		/// <seealso cref="MouseRangeStart" />
		/// <seealso cref="MouseRangeEnd" />
		public static bool IsMouse(this InputCode code)
		{
			var value = (int)code;
			return (value >= MouseRangeStart) && (value <= MouseRangeEnd);
		}

		/// <summary>
		///     Returns <c>true</c> if given <see cref="InputCode" /> is a gamepad code.
		/// </summary>
		/// <param name="code">The code to check.</param>
		/// <returns>Whether the code is a gamepad code.</returns>
		/// <seealso cref="PadRangeStart" />
		/// <seealso cref="PadRangeEnd" />
		public static bool IsGamepad(this InputCode code)
		{
			var value = (int)code;
			return (value >= PadRangeStart) && (value <= PadRangeEnd);
		}
	}
}
