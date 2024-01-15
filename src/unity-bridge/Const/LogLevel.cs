// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

using System.Diagnostics.CodeAnalysis;

namespace RavingBots.MultiInput
{
	/// <summary>
	///     Logging level constants.
	/// </summary>
	[SuppressMessage("ReSharper", "UnusedMember.Global")]
	public enum LogLevel
	{
		/// <summary>
		///     Really verbose messages (used by the debug version of this library).
		/// </summary>
		DebugVerbose = 0,

		/// <summary>
		///     Verbose messages (used by the debug version of this library).
		/// </summary>
		Debug = 1,

		/// <summary>
		///     Information messages.
		/// </summary>
		Info = 2,

		/// <summary>
		///     Warnings.
		/// </summary>
		Warning = 3,

		/// <summary>
		///     Errors.
		/// </summary>
		Error = 4,

		/// <summary>
		///     Highest possible value, used to disable logging.
		/// </summary>
		None = 5
	}
}
