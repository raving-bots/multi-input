// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

using System;

namespace RavingBots.MultiInput
{
	internal class Logger
	{
		private readonly LogLevel _level;

		internal Logger(LogLevel level)
		{
			_level = level;
		}

		internal void Callback(IntPtr userData, LogLevel level, IntPtr messagePtr)
		{
			if (level < _level)
			{
				// duplicates the check in Record but avoids
				// transcoding for filtered-out messages
				return;
			}

			var message = Native.Decode(messagePtr);
			Record(level, message);
		}

		private void Record(LogLevel level, string message)
		{
			if (level < _level)
			{
				return;
			}

			const string prefix = "[Multi-Input]";

			// ReSharper disable once SwitchStatementMissingSomeCases
			switch (level)
			{
				case LogLevel.DebugVerbose:
					UnityEngine.Debug.Log($"{prefix} [VERBOSE] {message}");
					break;
				case LogLevel.Debug:
					UnityEngine.Debug.Log($"{prefix} [DEBUG] {message}");
					break;
				case LogLevel.Info:
					UnityEngine.Debug.Log($"{prefix} {message}");
					break;
				case LogLevel.Warning:
					UnityEngine.Debug.LogWarning($"{prefix} {message}");
					break;
				case LogLevel.Error:
					UnityEngine.Debug.LogError($"{prefix} {message}");
					break;
			}
		}

		internal void Verbose(string message)
		{
			Record(LogLevel.DebugVerbose, message);
		}

		internal void Debug(string message)
		{
			Record(LogLevel.Debug, message);
		}

		internal void Info(string message)
		{
			Record(LogLevel.Info, message);
		}

		internal void Warning(string message)
		{
			Record(LogLevel.Warning, message);
		}

		internal void Error(string message)
		{
			Record(LogLevel.Error, message);
		}
	}
}
