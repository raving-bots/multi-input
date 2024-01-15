// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

using System;
using System.Collections.Generic;

namespace RavingBots.MultiInput
{
	internal sealed class NativeDevice : IDevice
	{
		private readonly IntPtr _context;
		private readonly Logger _log;
		private readonly Dictionary<InputCode, NativeVirtualAxis> _axisCache;

		public long Id { get; }
		public string Name { get; }
		public string Location { get; }
		public string InternalName { get; }
		public int VendorId { get; }
		public int ProductId { get; }
		public int Revision { get; }
		public string Serial { get; }
		public bool IsUsable => Native.IsUsable(_context, Id);
		public bool CanVibrate => Native.CanVibrate(_context, Id);

		public IVirtualAxis this[InputCode code]
		{
			get
			{
				if (!Native.HasAxis(_context, Id, code))
				{
					return null;
				}

				NativeVirtualAxis axis;

				// ReSharper disable once InvertIf
				if (!_axisCache.TryGetValue(code, out axis))
				{
					axis = new NativeVirtualAxis(_context, Id, code);
					_axisCache.Add(code, axis);
				}

				return axis;
			}
		}

		public IEnumerable<InputCode> SupportedAxes
		{
			get
			{
				var count = Native.GetAxisCount(_context, Id);
				var axes = new InputCode[count];

				if (Native.GetAxes(_context, Id, axes, count))
				{
					return axes;
				}

				_log.Error($"Failed to grab supported axes for device {Id} ({Name})");
				return null;
			}
		}

		internal NativeDevice(Logger log, IntPtr context, long id)
		{
			_log = log;
			_context = context;
			_axisCache = new Dictionary<InputCode, NativeVirtualAxis>();

			Id = id;

			var info = new Native.ApiDevice();
			if (Native.GetDevice(context, id, ref info))
			{
				Name = Native.Decode(info.Name);
				Location = Native.Decode(info.LocationId);
				InternalName = Native.Decode(info.InternalId);
				Serial = Native.Decode(info.Serial);
				VendorId = info.VendorId;
				ProductId = info.ProductId;
				Revision = info.Revision;
			}
		}

		public bool Vibrate(int duration, float left, float right)
		{
			return Native.Vibrate(_context, Id, duration, left, right);
		}

		public void Reset()
		{
			Native.ResetDevice(_context, Id);
		}

		public override string ToString()
		{
			if (!IsUsable)
			{
				return $"#{Id} '{Name}': unusable";
			}

			var serial = !string.IsNullOrEmpty(Serial) ? $"'{Serial}'" : "<no serial number>";
			var @internal = !string.IsNullOrEmpty(InternalName) ? $"'{InternalName}'" : "<no internal name>";
			var location = !string.IsNullOrEmpty(Location) ? $"'{Location}'" : "<no location>";
			var product = ProductId != 0 ? $"0x{ProductId:X04}" : "<no product ID>";
			var vendor = VendorId != 0 ? $"0x{VendorId:X04}" : "<no vendor ID>";
			var revision = Revision != 0 ? $"0x{Revision:X04}" : "<no revision>";

			return $"#{Id} '{Name}' (internal: {@internal}, location: {location}): " +
				$"usable, product: {product}, vendor: {vendor}, revision: {revision}, serial: {serial}";
		}
	}
}
