// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

using System.Diagnostics.CodeAnalysis;

namespace RavingBots.MultiInput
{
	/// <summary>
	///     Platform- and device-independent virtual axis codes.
	/// </summary>
	/// <remarks>
	///     <para>
	///         This enumeration is guaranteed to be forward- and backward-compatible,
	///         and so can be safely serialized. <see cref="InputCodeExt" /> contains few utility
	///         extension methods related to <c>InputCode</c>.
	///     </para>
	///     <para>
	///         Not all codes present here will actually
	///         be reported � it highly depends on what the OS and the actual device supports. You should
	///         keep that in mind when determining default controls for your game.
	///     </para>
	///     <note type="important">
	///         Keyboard codes are based on US International layout. There are several so-called OEM keys which might have
	///         different
	///         purpose on some layouts (those will be marked in remarks). We currently don't process or expose keyboard layout
	///         information,
	///         only report the codes as-is, based on what we get from the OS.
	///         This might affect accuracy of UI when presenting the keys based on the input codes in localized settings.
	///     </note>
	///     <para>
	///         There are three enumerator value ranges:
	///         <list type="bullet">
	///             <item>
	///                 All keyboard codes have values between <see cref="InputCodeExt.KeyboardRangeStart" /> and
	///                 <see cref="InputCodeExt.KeyboardRangeEnd" /> (inclusive).
	///             </item>
	///             <item>
	///                 All mouse codes have values between <see cref="InputCodeExt.MouseRangeStart" /> and
	///                 <see cref="InputCodeExt.MouseRangeStart" /> (inclusive).
	///             </item>
	///             <item>
	///                 All gamepad codes have values between <see cref="InputCodeExt.PadRangeStart" /> and
	///                 <see cref="InputCodeExt.PadRangeStart" /> (inclusive).
	///             </item>
	///         </list>
	///     </para>
	///     <para>
	///         Additionally, three subranges are available for your use, if you need to store
	///         custom codes:
	///     </para>
	///     <list type="bullet">
	///         <item>
	///             Custom keyboard codes should have values between <see cref="InputCodeExt.KeyboardCustomRangeStart" /> and
	///             <see cref="InputCodeExt.KeyboardRangeEnd" /> (inclusive).
	///         </item>
	///         <item>
	///             Custom mouse codes have should values between <see cref="InputCodeExt.MouseCustomRangeStart" /> and
	///             <see cref="InputCodeExt.MouseRangeStart" /> (inclusive).
	///         </item>
	///         <item>
	///             Custom gamepad codes have should values between <see cref="InputCodeExt.PadCustomRangeStart" /> and
	///             <see cref="InputCodeExt.PadRangeStart" /> (inclusive).
	///         </item>
	///     </list>
	///     <para>
	///         Axes representing digital buttons always report 0 or 1 value. Other axes will report scalar floating point
	///         values.
	///         The range of analog axis values depends on the axis (for example <see cref="MouseX" /> range is unbounded, and
	///         <see cref="PadLeftStickX" /> range is <c>[-1, 1]</c>) � see the description of the enumerator for details.
	///         Range notation has been used throughout this document as a shorthand:
	///     </para>
	///     <list type="bullet">
	///         <item><c>[x, y]</c> means "from x (inclusive) to y (inclusive)"</item>
	///         <item><c>[x, y)</c> means "from x (inclusive) to y (exclusive)"</item>
	///         <item><c>(x, y]</c> means "from x (exclusive) to y (inclusive)"</item>
	///         <item><c>(x, y)</c> means "from x (exclusive) to y (exclusive)"</item>
	///     </list>
	/// </remarks>
	/// <seealso cref="InputCodeExt" />
	[SuppressMessage("ReSharper", "UnusedMember.Global")]
	public enum InputCode
	{
		// TODO this needs to be generated from input_code.hpp
		// MAINTAINER NOTE:
		// This enumeration needs to be serialization-friendly.
		// (ranges are inclusive)
		//
		// - Keyboard range is 100000 to 149999 (reserved custom 150000-199999)
		// - Mouse range is 200000 to 249999 (reserved custom 250000-299999)
		// - Gamepad range is 300000 to 349999 (reserved custom 350000-399999)
		// - Always add new values at the end of the respective range
		// - Never reuse numeric values
		// - Never remove enumerators
		// - Never rename enumerators
		// - Never change numeric values of existing enumerators
		// - Never assign values from reserved subranges

		/// <summary>
		///     Value reserved for unknown codes or uninitialized variables.
		/// </summary>
		None = 0,

		/// <summary>
		///     The <c>0</c> key (alphanumeric).
		/// </summary>
		Key0 = 100000,

		/// <summary>
		///     The <c>1</c> key (alphanumeric).
		/// </summary>
		Key1 = 100001,

		/// <summary>
		///     The <c>2</c> key (alphanumeric).
		/// </summary>
		Key2 = 100002,

		/// <summary>
		///     The <c>3</c> key (alphanumeric).
		/// </summary>
		Key3 = 100003,

		/// <summary>
		///     The <c>4</c> key (alphanumeric).
		/// </summary>
		Key4 = 100004,

		/// <summary>
		///     The <c>5</c> key (alphanumeric).
		/// </summary>
		Key5 = 100005,

		/// <summary>
		///     The <c>6</c> key (alphanumeric).
		/// </summary>
		Key6 = 100006,

		/// <summary>
		///     The <c>7</c> key (alphanumeric).
		/// </summary>
		Key7 = 100007,

		/// <summary>
		///     The <c>8</c> key (alphanumeric).
		/// </summary>
		Key8 = 100008,

		/// <summary>
		///     The <c>9</c> key (alphanumeric).
		/// </summary>
		Key9 = 100009,

		/// <summary>
		///     The <c>A</c> key.
		/// </summary>
		KeyA = 100010,

		/// <summary>
		///     The <c>B</c> key.
		/// </summary>
		KeyB = 100011,

		/// <summary>
		///     The <c>C</c> key.
		/// </summary>
		KeyC = 100012,

		/// <summary>
		///     The <c>D</c> key.
		/// </summary>
		KeyD = 100013,

		/// <summary>
		///     The <c>E</c> key.
		/// </summary>
		KeyE = 100014,

		/// <summary>
		///     The <c>F</c> key.
		/// </summary>
		KeyF = 100015,

		/// <summary>
		///     The <c>G</c> key.
		/// </summary>
		KeyG = 100016,

		/// <summary>
		///     The <c>H</c> key.
		/// </summary>
		KeyH = 100017,

		/// <summary>
		///     The <c>I</c> key.
		/// </summary>
		KeyI = 100018,

		/// <summary>
		///     The <c>J</c> key.
		/// </summary>
		KeyJ = 100019,

		/// <summary>
		///     The <c>K</c> key.
		/// </summary>
		KeyK = 100020,

		/// <summary>
		///     The <c>L</c> key.
		/// </summary>
		KeyL = 100021,

		/// <summary>
		///     The <c>M</c> key.
		/// </summary>
		KeyM = 100022,

		/// <summary>
		///     The <c>N</c> key.
		/// </summary>
		KeyN = 100023,

		/// <summary>
		///     The <c>O</c> key.
		/// </summary>
		KeyO = 100024,

		/// <summary>
		///     The <c>P</c> key.
		/// </summary>
		KeyP = 100025,

		/// <summary>
		///     The <c>Q</c> key.
		/// </summary>
		KeyQ = 100026,

		/// <summary>
		///     The <c>R</c> key.
		/// </summary>
		KeyR = 100027,

		/// <summary>
		///     The <c>S</c> key.
		/// </summary>
		KeyS = 100028,

		/// <summary>
		///     The <c>T</c> key.
		/// </summary>
		KeyT = 100029,

		/// <summary>
		///     The <c>U</c> key.
		/// </summary>
		KeyU = 100030,

		/// <summary>
		///     The <c>V</c> key.
		/// </summary>
		KeyV = 100031,

		/// <summary>
		///     The <c>W</c> key.
		/// </summary>
		KeyW = 100032,

		/// <summary>
		///     The <c>X</c> key.
		/// </summary>
		KeyX = 100033,

		/// <summary>
		///     The <c>Y</c> key.
		/// </summary>
		KeyY = 100034,

		/// <summary>
		///     The <c>Z</c> key.
		/// </summary>
		KeyZ = 100035,

		/// <summary>
		///     The OEM 1 key: <c>;</c> (semicolon) and <c>:</c> (colon) on the US International layout.
		/// </summary>
		KeySemicolon = 100036,

		/// <summary>
		///     The OEM 2 key: <c>/</c> (slash) and <c>?</c> (question mark) on the US International layout.
		/// </summary>
		KeySlash = 100037,

		/// <summary>
		///     The OEM 3 key: <c>`</c> (grave accent/backtick) and <c>~</c> (tilde) on the US International layout.
		/// </summary>
		KeyAccent = 100038,

		/// <summary>
		///     The OEM 4 key: <c>[</c> (left bracket) and <c>{</c> (left brace) on the US International layout.
		/// </summary>
		KeyLeftBracket = 100039,

		/// <summary>
		///     The OEM 5 key: <c>\</c> (backslash) and <c>|</c> (vertical bar/pipe) on the US International layout.
		/// </summary>
		KeyBackslash = 100040,

		/// <summary>
		///     The OEM 6 key: <c>]</c> (right bracket) and <c>}</c> (right brace) on the US International layout.
		/// </summary>
		KeyRightBracket = 100041,

		/// <summary>
		///     The OEM 7 key: <c>'</c> (apostrophe/single quote) and <c>"</c> (double quote) on the US International layout.
		/// </summary>
		KeyQuote = 100042,

		/// <summary>
		///     The OEM 8 key: not present on the US International layout.
		/// </summary>
		KeyOem8 = 100043,

		/// <summary>
		///     The OEM 102 key: additional <see cref="KeyBackslash" /> on the US International layout.
		/// </summary>
		KeyOem102 = 100044,

		/// <summary>
		///     The Backspace key.
		/// </summary>
		KeyBackspace = 100045,

		/// <summary>
		///     The Tab key.
		/// </summary>
		KeyTab = 100046,

		/// <summary>
		///     The Clear key (Apple keyboards).
		/// </summary>
		KeyClear = 100047,

		/// <summary>
		///     The primary Enter key.
		/// </summary>
		KeyEnter = 100048,

		/// <summary>
		///     The Escape key.
		/// </summary>
		KeyEscape = 100049,

		/// <summary>
		///     The Space key.
		/// </summary>
		KeySpace = 100050,

		/// <summary>
		///     The OEM plus key: <c>+</c> (plus sign) and <c>=</c> (equals sign) on the US International layout.
		/// </summary>
		KeyPlus = 100051,

		/// <summary>
		///     The OEM comma key: <c>,</c> (comma) and <c>&lt;</c> (left angle bracket) on the US International layout.
		/// </summary>
		KeyComma = 100052,

		/// <summary>
		///     The OEM minus key: <c>-</c> (minus sign) and <c>_</c> (underscore) on the US International layout.
		/// </summary>
		KeyMinus = 100053,

		/// <summary>
		///     The OEM period key: <c>.</c> (period/full stop) and <c>&gt;</c> (right angle bracket) on the US International
		///     layout.
		/// </summary>
		KeyPeriod = 100054,

		/// <summary>
		///     The <c>0</c> key (numeric pad).
		/// </summary>
		KeyNum0 = 100055,

		/// <summary>
		///     The <c>1</c> key (numeric pad).
		/// </summary>
		KeyNum1 = 100056,

		/// <summary>
		///     The <c>2</c> key (numeric pad).
		/// </summary>
		KeyNum2 = 100057,

		/// <summary>
		///     The <c>3</c> key (numeric pad).
		/// </summary>
		KeyNum3 = 100058,

		/// <summary>
		///     The <c>4</c> key (numeric pad).
		/// </summary>
		KeyNum4 = 100059,

		/// <summary>
		///     The <c>5</c> key (numeric pad).
		/// </summary>
		KeyNum5 = 100060,

		/// <summary>
		///     The <c>6</c> key (numeric pad).
		/// </summary>
		KeyNum6 = 100061,

		/// <summary>
		///     The <c>7</c> key (numeric pad).
		/// </summary>
		KeyNum7 = 100062,

		/// <summary>
		///     The <c>8</c> key (numeric pad).
		/// </summary>
		KeyNum8 = 100063,

		/// <summary>
		///     The <c>9</c> key (numeric pad).
		/// </summary>
		KeyNum9 = 100064,

		/// <summary>
		///     The decimal key (numeric pad).
		/// </summary>
		KeyNumDecimal = 100065,

		/// <summary>
		///     The <c>/</c> (divide) key (numeric pad).
		/// </summary>
		KeyNumDivide = 100066,

		/// <summary>
		///     The <c>*</c> (multiply) key (numeric pad).
		/// </summary>
		KeyNumMultiply = 100067,

		/// <summary>
		///     The <c>-</c> (subtract) key (numeric pad).
		/// </summary>
		KeyNumMinus = 100068,

		/// <summary>
		///     The <c>+</c> (add) key (numeric pad).
		/// </summary>
		KeyNumPlus = 100069,

		/// <summary>
		///     The Enter key (numeric pad).
		/// </summary>
		KeyNumEnter = 100070,

		/// <summary>
		///     The up arrow key.
		/// </summary>
		KeyUpArrow = 100071,

		/// <summary>
		///     The down arrow key.
		/// </summary>
		KeyDownArrow = 100072,

		/// <summary>
		///     The right arrow key.
		/// </summary>
		KeyRightArrow = 100073,

		/// <summary>
		///     The left arrow key.
		/// </summary>
		KeyLeftArrow = 100074,

		/// <summary>
		///     The Insert key.
		/// </summary>
		KeyInsert = 100075,

		/// <summary>
		///     The Delete key.
		/// </summary>
		KeyDelete = 100076,

		/// <summary>
		///     The Home key.
		/// </summary>
		KeyHome = 100077,

		/// <summary>
		///     The End key.
		/// </summary>
		KeyEnd = 100078,

		/// <summary>
		///     The Page Up key.
		/// </summary>
		KeyPageUp = 100079,

		/// <summary>
		///     The Page Down key.
		/// </summary>
		KeyPageDown = 100080,

		/// <summary>
		///     The F1 key.
		/// </summary>
		KeyF1 = 100081,

		/// <summary>
		///     The F2 key.
		/// </summary>
		KeyF2 = 100082,

		/// <summary>
		///     The F3 key.
		/// </summary>
		KeyF3 = 100083,

		/// <summary>
		///     The F4 key.
		/// </summary>
		KeyF4 = 100084,

		/// <summary>
		///     The F5 key.
		/// </summary>
		KeyF5 = 100085,

		/// <summary>
		///     The F6 key.
		/// </summary>
		KeyF6 = 100086,

		/// <summary>
		///     The F7 key.
		/// </summary>
		KeyF7 = 100087,

		/// <summary>
		///     The F8 key.
		/// </summary>
		KeyF8 = 100088,

		/// <summary>
		///     The F9 key.
		/// </summary>
		KeyF9 = 100089,

		/// <summary>
		///     The F10 key.
		/// </summary>
		KeyF10 = 100090,

		/// <summary>
		///     The F11 key.
		/// </summary>
		KeyF11 = 100091,

		/// <summary>
		///     The F12 key.
		/// </summary>
		KeyF12 = 100092,

		/// <summary>
		///     The F13 key.
		/// </summary>
		KeyF13 = 100093,

		/// <summary>
		///     The F14 key.
		/// </summary>
		KeyF14 = 100094,

		/// <summary>
		///     The F15 key.
		/// </summary>
		KeyF15 = 100095,

		/// <summary>
		///     The F16 key.
		/// </summary>
		KeyF16 = 100096,

		/// <summary>
		///     The F17 key.
		/// </summary>
		KeyF17 = 100097,

		/// <summary>
		///     The F18 key.
		/// </summary>
		KeyF18 = 100098,

		/// <summary>
		///     The F19 key.
		/// </summary>
		KeyF19 = 100099,

		/// <summary>
		///     The F20 key.
		/// </summary>
		KeyF20 = 100100,

		/// <summary>
		///     The F21 key.
		/// </summary>
		KeyF21 = 100101,

		/// <summary>
		///     The F22 key.
		/// </summary>
		KeyF22 = 100102,

		/// <summary>
		///     The F23 key.
		/// </summary>
		KeyF23 = 100103,

		/// <summary>
		///     The F24 key.
		/// </summary>
		KeyF24 = 100104,

		/// <summary>
		///     The right Shift key.
		/// </summary>
		KeyRightShift = 100105,

		/// <summary>
		///     The left Shift key.
		/// </summary>
		KeyLeftShift = 100106,

		/// <summary>
		///     The right Alt (Option on Apple keyboards) key.
		/// </summary>
		KeyRightAlt = 100107,

		/// <summary>
		///     The left Alt (Option on Apple keyboards) key.
		/// </summary>
		KeyLeftAlt = 100108,

		/// <summary>
		///     The right Control key.
		/// </summary>
		KeyRightControl = 100109,

		/// <summary>
		///     The left Control key.
		/// </summary>
		KeyLeftControl = 100110,

		/// <summary>
		///     The right Windows (Command on Apple keyboards) key.
		/// </summary>
		KeyRightCommand = 100111,

		/// <summary>
		///     The left Windows (Command on Apple keyboards) key.
		/// </summary>
		KeyLeftCommand = 100112,

		/// <summary>
		///     The Application key.
		/// </summary>
		KeyApps = 100113,

		/// <summary>
		///     The Print Screen key.
		/// </summary>
		KeyPrintScreen = 100114,

		/// <summary>
		///     The Pause key.
		/// </summary>
		KeyPause = 100115,

		/// <summary>
		///     The CapsLock key.
		/// </summary>
		KeyCapsLock = 100116,

		/// <summary>
		///     The NumLock key.
		/// </summary>
		KeyNumLock = 100117,

		/// <summary>
		///     The ScrollLock key.
		/// </summary>
		KeyScrollLock = 100118,

		/// <summary>
		///     The left mouse button.
		/// </summary>
		MouseLeft = 200000,

		/// <summary>
		///     The right mouse button.
		/// </summary>
		MouseRight = 200001,

		/// <summary>
		///     The middle mouse button.
		/// </summary>
		MouseMiddle = 200002,

		/// <summary>
		///     The fourth mouse button.
		/// </summary>
		MouseFourth = 200003,

		/// <summary>
		///     The fifth mouse button.
		/// </summary>
		MouseFifth = 200004,

		/// <summary>
		///     The sixth mouse button.
		/// </summary>
		MouseSixth = 200005,

		/// <summary>
		///     The seventh mouse button.
		/// </summary>
		MouseSeventh = 200006,

		/// <summary>
		///     The derived mouse <c>X-</c> axis. Value is relative mouse movement to the left.
		///     Range <c>[0, <see cref="float.MaxValue">MaxValue</see>]</c>.
		/// </summary>
		/// <seealso cref="MouseX" />
		MouseXLeft = 200007,

		/// <summary>
		///     The derived mouse <c>X+</c> axis. Value is relative mouse movement to the right.
		///     Range <c>[0, <see cref="float.MaxValue">MaxValue</see>]</c>.
		/// </summary>
		/// <seealso cref="MouseX" />
		MouseXRight = 200008,

		/// <summary>
		///     The derived mouse <c>Y+</c> axis. Value is relative mouse movement away from the user.
		///     Range <c>[0, <see cref="float.MaxValue">MaxValue</see>]</c>.
		/// </summary>
		/// <seealso cref="MouseX" />
		MouseYUp = 200009,

		/// <summary>
		///     The derived mouse <c>Y-</c> axis. Value is relative mouse movement towards the user.
		///     Range <c>[0, <see cref="float.MaxValue">MaxValue</see>]</c>.
		/// </summary>
		/// <seealso cref="MouseY" />
		MouseYDown = 200010,

		/// <summary>
		///     The derived mouse <c>Z+</c> axis (the wheel). Value is relative mouse movement away from the user.
		///     Range <c>[0, <see cref="float.MaxValue">MaxValue</see>]</c>.
		/// </summary>
		/// <seealso cref="MouseWheel" />
		MouseWheelUp = 200011,

		/// <summary>
		///     The derived mouse <c>Z-</c> axis (the wheel). Value is relative mouse movement towards the user.
		///     Range <c>[0, <see cref="float.MaxValue">MaxValue</see>]</c>.
		/// </summary>
		/// <seealso cref="MouseWheel" />
		MouseWheelDown = 200012,

		/// <summary>
		///     The mouse X axis. Value is relative horizontal mouse movement. Unbounded range
		///     (<c>[<see cref="float.MinValue">MinValue</see>, <see cref="float.MaxValue">MaxValue</see>]</c>).
		/// </summary>
		MouseX = 200013,

		/// <summary>
		///     The mouse Y axis. Value is relative vertical mouse movement. Unbounded range
		///     (<c>[<see cref="float.MinValue">MinValue</see>, <see cref="float.MaxValue">MaxValue</see>]</c>).
		/// </summary>
		MouseY = 200014,

		/// <summary>
		///     The mouse Z axis (the wheel). Value is relative wheel movement. Unbounded range
		///     (<c>[<see cref="float.MinValue">MinValue</see>, <see cref="float.MaxValue">MaxValue</see>]</c>).
		/// </summary>
		MouseWheel = 200015,

		/// <summary>
		///     The derived left analog stick Y+ axis. Non-zero when stick moved up/away from the user.
		///     Range <c>[0, 1]</c>.
		/// </summary>
		/// <seealso cref="PadLeftStickY" />
		PadLeftStickUp = 300000,

		/// <summary>
		///     The derived left analog stick Y- axis. Non-zero when stick moved down/towards from the user.
		///     Range <c>[0, 1]</c>.
		/// </summary>
		/// <seealso cref="PadLeftStickY" />
		PadLeftStickDown = 300001,

		/// <summary>
		///     The derived left analog stick X- axis. Non-zero when stick moved to the left.  Range <c>[0, 1]</c>.
		/// </summary>
		/// <seealso cref="PadLeftStickX" />
		PadLeftStickLeft = 300002,

		/// <summary>
		///     The derived left analog stick X+ axis. Non-zero when stick moved to the right.  Range <c>[0, 1]</c>.
		/// </summary>
		/// <seealso cref="PadLeftStickX" />
		PadLeftStickRight = 300003,

		/// <summary>
		///     The gamepad left stick digital button.
		/// </summary>
		PadLeftStick = 300004,

		/// <summary>
		///     The gamepad left analog stick X axis. Value is position of the stick in the horizontal axis.
		///     Range <c>[-1, 1]</c>.
		/// </summary>
		PadLeftStickX = 300005,

		/// <summary>
		///     The gamepad left analog stick Y axis. Value is position of the stick in the vertical axis.
		///     Range <c>[-1, 1]</c>.
		/// </summary>
		PadLeftStickY = 300006,

		/// <summary>
		///     The derived right analog stick Y+ axis. Non-zero when stick moved up/away from the user.
		///     Range <c>[0, 1]</c>.
		/// </summary>
		/// <seealso cref="PadRightStickY" />
		PadRightStickUp = 300007,

		/// <summary>
		///     The derived right analog stick Y- axis. Non-zero when stick moved down/towards from the user.
		///     Range <c>[0, 1]</c>.
		/// </summary>
		/// <seealso cref="PadRightStickY" />
		PadRightStickDown = 300008,

		/// <summary>
		///     The derived right analog stick X- axis. Non-zero when stick moved to the left.  Range <c>[0, 1]</c>.
		/// </summary>
		/// <seealso cref="PadRightStickX" />
		PadRightStickLeft = 300009,

		/// <summary>
		///     The derived right analog stick X+ axis. Non-zero when stick moved to the right.  Range <c>[0, 1]</c>.
		/// </summary>
		/// <seealso cref="PadRightStickX" />
		PadRightStickRight = 300010,

		/// <summary>
		///     The gamepad right stick digital button.
		/// </summary>
		PadRightStick = 300011,

		/// <summary>
		///     The gamepad right analog stick X axis. Value is position of the stick in the horizontal axis.
		///     Range <c>[-1, 1]</c>.
		/// </summary>
		PadRightStickX = 300012,

		/// <summary>
		///     The gamepad right analog stick Y axis. Value is position of the stick in the vertical axis.
		///     Range <c>[-1, 1]</c>.
		/// </summary>
		PadRightStickY = 300013,

		/// <summary>
		///     The gamepad up button (DPad).
		/// </summary>
		PadDPadUp = 300014,

		/// <summary>
		///     The gamepad down button (DPad).
		/// </summary>
		PadDPadDown = 300015,

		/// <summary>
		///     The gamepad left button (DPad).
		/// </summary>
		PadDPadLeft = 300016,

		/// <summary>
		///     The gamepad right button (DPad).
		/// </summary>
		PadDPadRight = 300017,

		/// <summary>
		///     The derived analog DPad X axis.
		///     Value is one of -1 (when <see cref="PadDPadLeft" /> is pressed),
		///     1 (when <see cref="PadDPadRight" /> is pressed) or 0 (when neither are pressed).
		/// </summary>
		PadDPadX = 300018,

		/// <summary>
		///     The derived analog DPad Y axis. Value is one of -1 (when <see cref="PadDPadDown" /> is pressed),
		///     1 (when <see cref="PadDPadUp" /> is pressed) or 0 (when neither are pressed).
		/// </summary>
		PadDPadY = 300019,

		/// <summary>
		///     The gamepad A button (cross on PS4 gamepads).
		/// </summary>
		PadA = 300020,

		/// <summary>
		///     The gamepad B button (circle on PS4 gamepads).
		/// </summary>
		PadB = 300021,

		/// <summary>
		///     The gamepad X button (square on PS4 gamepads).
		/// </summary>
		PadX = 300022,

		/// <summary>
		///     The gamepad Y button (triangle on PS4 gamepads).
		/// </summary>
		PadY = 300023,

		/// <summary>
		///     The gamepad left trigger analog axis (L2 on PS4 gamepads). Range <c>[0, 1]</c>.
		/// </summary>
		PadLeftTrigger = 300024,

		/// <summary>
		///     The gamepad right trigger analog axis (R2 on PS4 gamepads). Range <c>[0, 1]</c>.
		/// </summary>
		PadRightTrigger = 300025,

		/// <summary>
		///     The gamepad left bumper/shoulder digital button (L1 on PS4 gamepads).
		/// </summary>
		PadLeftBumper = 300026,

		/// <summary>
		///     The gamepad right bumper/shoulder digital button (R1 on PS4 gamepads).
		/// </summary>
		PadRightBumper = 300027,

		/// <summary>
		///     The gamepad Back (Xbox360), View (XboxOne), Select (PS3) or Share (PS4) digital button.
		/// </summary>
		PadBack = 300028,

		/// <summary>
		///     The gamepad Start (Xbox360/PS3), Menu (XboxOne) or Options (PS4) digital button.
		/// </summary>
		PadStart = 300029
	}
}
