// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include <string>
#include <stdexcept>
#include <utility>
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/format.hpp>

#include <IOKit/IOReturn.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/usb/IOUSBLib.h>

#include <CF++.hpp>

#define RB_OSX_ENUMERATE_MODE CFSTR("RBMultiInputDeviceEnumerate")

namespace detail {
    inline std::string format(boost::format& fmt) {
        return fmt.str();
    }

    template <typename T, typename... Args>
    std::string format(boost::format& fmt, T&& arg, Args&&... args) {
        return format(fmt % std::forward<T>(arg), std::forward<Args>(args)...);
    }
}

template <typename... Args>
std::string format(std::string fmt_str, Args&&... args) {
    boost::format fmt{fmt_str};
    return detail::format(fmt, std::forward<Args>(args)...);
}

std::string get_collection_type(IOHIDElementRef element) {
    auto type = IOHIDElementGetCollectionType(element);
    switch (type) {
        case kIOHIDElementCollectionTypePhysical:
            return "Physical";
        case kIOHIDElementCollectionTypeApplication:
            return "Application";
        case kIOHIDElementCollectionTypeLogical:
            return "Logical";
        case kIOHIDElementCollectionTypeReport:
            return "Report";
        case kIOHIDElementCollectionTypeNamedArray:
            return "NamedArray";
        case kIOHIDElementCollectionTypeUsageSwitch:
            return "UsageSwitch";
        case kIOHIDElementCollectionTypeUsageModifier:
            return "UsageModifier";
        default:
            return format("Unknown (%1%)", type);
    }
}

std::string get_element_type(IOHIDElementRef element) {
    auto type = IOHIDElementGetType(element);
    switch (type) {
        case kIOHIDElementTypeInput_Misc:
            return "Input_Misc";
        case kIOHIDElementTypeInput_Button:
            return "Input_Button";
        case kIOHIDElementTypeInput_Axis:
            return "Input_Axis";
        case kIOHIDElementTypeInput_ScanCodes:
            return "Input_ScanCodes";
        case kIOHIDElementTypeOutput:
            return "Output";
        case kIOHIDElementTypeFeature:
            return "Feature";
        case kIOHIDElementTypeCollection:
            return format("Collection (%1%)", get_collection_type(element));
        default:
            return format("Unknown (%1%)", type);
    }
}

std::string get_indent(int level) {
    return std::string(level * 4, ' ');
}

std::string get_undefined_usage(uint32_t usage) {
    return format("Unknown usage (0x%|02X|)", usage);
}

std::string get_usage(IOHIDElementRef element) {
    auto page = IOHIDElementGetUsagePage(element);
    auto usage = IOHIDElementGetUsage(element);

    switch (page) {
        case kHIDPage_GenericDesktop:
            switch (usage) {
                case kHIDUsage_GD_Pointer:
                    return "GD_Pointer";
                case kHIDUsage_GD_Mouse:
                    return "GD_Mouse";
                case kHIDUsage_GD_Joystick:
                    return "GD_Joystick";
                case kHIDUsage_GD_GamePad:
                    return "GD_GamePad";
                case kHIDUsage_GD_Keyboard:
                    return "GD_Keyboard";
                case kHIDUsage_GD_Keypad:
                    return "GD_Keypad";
                case kHIDUsage_GD_MultiAxisController:
                    return "GD_MultiAxisController";
                case kHIDUsage_GD_X:
                    return "GD_X";
                case kHIDUsage_GD_Y:
                    return "GD_Y";
                case kHIDUsage_GD_Z:
                    return "GD_Z";
                case kHIDUsage_GD_Rx:
                    return "GD_Rx";
                case kHIDUsage_GD_Ry:
                    return "GD_Ry";
                case kHIDUsage_GD_Rz:
                    return "GD_Rz";
                case kHIDUsage_GD_Slider:
                    return "GD_Slider";
                case kHIDUsage_GD_Dial:
                    return "GD_Dial";
                case kHIDUsage_GD_Wheel:
                    return "GD_Wheel";
                case kHIDUsage_GD_Hatswitch:
                    return "GD_Hatswitch";
                case kHIDUsage_GD_CountedBuffer:
                    return "GD_CountedBuffer";
                case kHIDUsage_GD_ByteCount:
                    return "GD_ByteCount";
                case kHIDUsage_GD_MotionWakeup:
                    return "GD_MotionWakeup";
                case kHIDUsage_GD_Start:
                    return "GD_Start";
                case kHIDUsage_GD_Select:
                    return "GD_Select";
                case kHIDUsage_GD_Vx:
                    return "GD_Vx";
                case kHIDUsage_GD_Vy:
                    return "GD_Vy";
                case kHIDUsage_GD_Vz:
                    return "GD_Vz";
                case kHIDUsage_GD_Vbrx:
                    return "GD_Vbrx";
                case kHIDUsage_GD_Vbry:
                    return "GD_Vbry";
                case kHIDUsage_GD_Vbrz:
                    return "GD_Vbrz";
                case kHIDUsage_GD_Vno:
                    return "GD_Vno";
                case kHIDUsage_GD_SystemControl:
                    return "GD_SystemControl";
                case kHIDUsage_GD_SystemPowerDown:
                    return "GD_SystemPowerDown";
                case kHIDUsage_GD_SystemSleep:
                    return "GD_SystemSleep";
                case kHIDUsage_GD_SystemWakeUp:
                    return "GD_SystemWakeUp";
                case kHIDUsage_GD_SystemContextMenu:
                    return "GD_SystemContextMenu";
                case kHIDUsage_GD_SystemMainMenu:
                    return "GD_SystemMainMenu";
                case kHIDUsage_GD_SystemAppMenu:
                    return "GD_SystemAppMenu";
                case kHIDUsage_GD_SystemMenuHelp:
                    return "GD_SystemMenuHelp";
                case kHIDUsage_GD_SystemMenuExit:
                    return "GD_SystemMenuExit";
                case kHIDUsage_GD_SystemMenu:
                    return "GD_SystemMenu";
                case kHIDUsage_GD_SystemMenuRight:
                    return "GD_SystemMenuRight";
                case kHIDUsage_GD_SystemMenuLeft:
                    return "GD_SystemMenuLeft";
                case kHIDUsage_GD_SystemMenuUp:
                    return "GD_SystemMenuUp";
                case kHIDUsage_GD_SystemMenuDown:
                    return "GD_SystemMenuDown";
                case kHIDUsage_GD_DPadUp:
                    return "GD_DPadUp";
                case kHIDUsage_GD_DPadDown:
                    return "GD_DPadDown";
                case kHIDUsage_GD_DPadRight:
                    return "GD_DPadRight";
                case kHIDUsage_GD_DPadLeft:
                    return "GD_DPadLeft";

                default:
                    return format("Unknown (%|02X|)", usage);
            }
        case kHIDPage_KeyboardOrKeypad:
            switch (usage) {
                case kHIDUsage_KeyboardErrorRollOver:
                    return "KeyboardErrorRollOver";
                case kHIDUsage_KeyboardPOSTFail:
                    return "KeyboardPOSTFail";
                case kHIDUsage_KeyboardErrorUndefined:
                    return "KeyboardErrorUndefined";
                case kHIDUsage_KeyboardA:
                    return "KeyboardA";
                case kHIDUsage_KeyboardB:
                    return "KeyboardB";
                case kHIDUsage_KeyboardC:
                    return "KeyboardC";
                case kHIDUsage_KeyboardD:
                    return "KeyboardD";
                case kHIDUsage_KeyboardE:
                    return "KeyboardE";
                case kHIDUsage_KeyboardF:
                    return "KeyboardF";
                case kHIDUsage_KeyboardG:
                    return "KeyboardG";
                case kHIDUsage_KeyboardH:
                    return "KeyboardH";
                case kHIDUsage_KeyboardI:
                    return "KeyboardI";
                case kHIDUsage_KeyboardJ:
                    return "KeyboardJ";
                case kHIDUsage_KeyboardK:
                    return "KeyboardK";
                case kHIDUsage_KeyboardL:
                    return "KeyboardL";
                case kHIDUsage_KeyboardM:
                    return "KeyboardM";
                case kHIDUsage_KeyboardN:
                    return "KeyboardN";
                case kHIDUsage_KeyboardO:
                    return "KeyboardO";
                case kHIDUsage_KeyboardP:
                    return "KeyboardP";
                case kHIDUsage_KeyboardQ:
                    return "KeyboardQ";
                case kHIDUsage_KeyboardR:
                    return "KeyboardR";
                case kHIDUsage_KeyboardS:
                    return "KeyboardS";
                case kHIDUsage_KeyboardT:
                    return "KeyboardT";
                case kHIDUsage_KeyboardU:
                    return "KeyboardU";
                case kHIDUsage_KeyboardV:
                    return "KeyboardV";
                case kHIDUsage_KeyboardW:
                    return "KeyboardW";
                case kHIDUsage_KeyboardX:
                    return "KeyboardX";
                case kHIDUsage_KeyboardY:
                    return "KeyboardY";
                case kHIDUsage_KeyboardZ:
                    return "KeyboardZ";
                case kHIDUsage_Keyboard1:
                    return "Keyboard1";
                case kHIDUsage_Keyboard2:
                    return "Keyboard2";
                case kHIDUsage_Keyboard3:
                    return "Keyboard3";
                case kHIDUsage_Keyboard4:
                    return "Keyboard4";
                case kHIDUsage_Keyboard5:
                    return "Keyboard5";
                case kHIDUsage_Keyboard6:
                    return "Keyboard6";
                case kHIDUsage_Keyboard7:
                    return "Keyboard7";
                case kHIDUsage_Keyboard8:
                    return "Keyboard8";
                case kHIDUsage_Keyboard9:
                    return "Keyboard9";
                case kHIDUsage_Keyboard0:
                    return "Keyboard0";
                case kHIDUsage_KeyboardReturnOrEnter:
                    return "KeyboardReturnOrEnter";
                case kHIDUsage_KeyboardEscape:
                    return "KeyboardEscape";
                case kHIDUsage_KeyboardDeleteOrBackspace:
                    return "KeyboardDeleteOrBackspace";
                case kHIDUsage_KeyboardTab:
                    return "KeyboardTab";
                case kHIDUsage_KeyboardSpacebar:
                    return "KeyboardSpacebar";
                case kHIDUsage_KeyboardHyphen:
                    return "KeyboardHyphen";
                case kHIDUsage_KeyboardEqualSign:
                    return "KeyboardEqualSign";
                case kHIDUsage_KeyboardOpenBracket:
                    return "KeyboardOpenBracket";
                case kHIDUsage_KeyboardCloseBracket:
                    return "KeyboardCloseBracket";
                case kHIDUsage_KeyboardBackslash:
                    return "KeyboardBackslash";
                case kHIDUsage_KeyboardNonUSPound:
                    return "KeyboardNonUSPound";
                case kHIDUsage_KeyboardSemicolon:
                    return "KeyboardSemicolon";
                case kHIDUsage_KeyboardQuote:
                    return "KeyboardQuote";
                case kHIDUsage_KeyboardGraveAccentAndTilde:
                    return "KeyboardGraveAccentAndTilde";
                case kHIDUsage_KeyboardComma:
                    return "KeyboardComma";
                case kHIDUsage_KeyboardPeriod:
                    return "KeyboardPeriod";
                case kHIDUsage_KeyboardSlash:
                    return "KeyboardSlash";
                case kHIDUsage_KeyboardCapsLock:
                    return "KeyboardCapsLock";
                case kHIDUsage_KeyboardF1:
                    return "KeyboardF1";
                case kHIDUsage_KeyboardF2:
                    return "KeyboardF2";
                case kHIDUsage_KeyboardF3:
                    return "KeyboardF3";
                case kHIDUsage_KeyboardF4:
                    return "KeyboardF4";
                case kHIDUsage_KeyboardF5:
                    return "KeyboardF5";
                case kHIDUsage_KeyboardF6:
                    return "KeyboardF6";
                case kHIDUsage_KeyboardF7:
                    return "KeyboardF7";
                case kHIDUsage_KeyboardF8:
                    return "KeyboardF8";
                case kHIDUsage_KeyboardF9:
                    return "KeyboardF9";
                case kHIDUsage_KeyboardF10:
                    return "KeyboardF10";
                case kHIDUsage_KeyboardF11:
                    return "KeyboardF11";
                case kHIDUsage_KeyboardF12:
                    return "KeyboardF12";
                case kHIDUsage_KeyboardPrintScreen:
                    return "KeyboardPrintScreen";
                case kHIDUsage_KeyboardScrollLock:
                    return "KeyboardScrollLock";
                case kHIDUsage_KeyboardPause:
                    return "KeyboardPause";
                case kHIDUsage_KeyboardInsert:
                    return "KeyboardInsert";
                case kHIDUsage_KeyboardHome:
                    return "KeyboardHome";
                case kHIDUsage_KeyboardPageUp:
                    return "KeyboardPageUp";
                case kHIDUsage_KeyboardDeleteForward:
                    return "KeyboardDeleteForward";
                case kHIDUsage_KeyboardEnd:
                    return "KeyboardEnd";
                case kHIDUsage_KeyboardPageDown:
                    return "KeyboardPageDown";
                case kHIDUsage_KeyboardRightArrow:
                    return "KeyboardRightArrow";
                case kHIDUsage_KeyboardLeftArrow:
                    return "KeyboardLeftArrow";
                case kHIDUsage_KeyboardDownArrow:
                    return "KeyboardDownArrow";
                case kHIDUsage_KeyboardUpArrow:
                    return "KeyboardUpArrow";
                case kHIDUsage_KeypadNumLock:
                    return "KeypadNumLock";
                case kHIDUsage_KeypadSlash:
                    return "KeypadSlash";
                case kHIDUsage_KeypadAsterisk:
                    return "KeypadAsterisk";
                case kHIDUsage_KeypadHyphen:
                    return "KeypadHyphen";
                case kHIDUsage_KeypadPlus:
                    return "KeypadPlus";
                case kHIDUsage_KeypadEnter:
                    return "KeypadEnter";
                case kHIDUsage_Keypad1:
                    return "Keypad1";
                case kHIDUsage_Keypad2:
                    return "Keypad2";
                case kHIDUsage_Keypad3:
                    return "Keypad3";
                case kHIDUsage_Keypad4:
                    return "Keypad4";
                case kHIDUsage_Keypad5:
                    return "Keypad5";
                case kHIDUsage_Keypad6:
                    return "Keypad6";
                case kHIDUsage_Keypad7:
                    return "Keypad7";
                case kHIDUsage_Keypad8:
                    return "Keypad8";
                case kHIDUsage_Keypad9:
                    return "Keypad9";
                case kHIDUsage_Keypad0:
                    return "Keypad0";
                case kHIDUsage_KeypadPeriod:
                    return "KeypadPeriod";
                case kHIDUsage_KeyboardNonUSBackslash:
                    return "KeyboardNonUSBackslash";
                case kHIDUsage_KeyboardApplication:
                    return "KeyboardApplication";
                case kHIDUsage_KeyboardPower:
                    return "KeyboardPower";
                case kHIDUsage_KeypadEqualSign:
                    return "KeypadEqualSign";
                case kHIDUsage_KeyboardF13:
                    return "KeyboardF13";
                case kHIDUsage_KeyboardF14:
                    return "KeyboardF14";
                case kHIDUsage_KeyboardF15:
                    return "KeyboardF15";
                case kHIDUsage_KeyboardF16:
                    return "KeyboardF16";
                case kHIDUsage_KeyboardF17:
                    return "KeyboardF17";
                case kHIDUsage_KeyboardF18:
                    return "KeyboardF18";
                case kHIDUsage_KeyboardF19:
                    return "KeyboardF19";
                case kHIDUsage_KeyboardF20:
                    return "KeyboardF20";
                case kHIDUsage_KeyboardF21:
                    return "KeyboardF21";
                case kHIDUsage_KeyboardF22:
                    return "KeyboardF22";
                case kHIDUsage_KeyboardF23:
                    return "KeyboardF23";
                case kHIDUsage_KeyboardF24:
                    return "KeyboardF24";
                case kHIDUsage_KeyboardExecute:
                    return "KeyboardExecute";
                case kHIDUsage_KeyboardHelp:
                    return "KeyboardHelp";
                case kHIDUsage_KeyboardMenu:
                    return "KeyboardMenu";
                case kHIDUsage_KeyboardSelect:
                    return "KeyboardSelect";
                case kHIDUsage_KeyboardStop:
                    return "KeyboardStop";
                case kHIDUsage_KeyboardAgain:
                    return "KeyboardAgain";
                case kHIDUsage_KeyboardUndo:
                    return "KeyboardUndo";
                case kHIDUsage_KeyboardCut:
                    return "KeyboardCut";
                case kHIDUsage_KeyboardCopy:
                    return "KeyboardCopy";
                case kHIDUsage_KeyboardPaste:
                    return "KeyboardPaste";
                case kHIDUsage_KeyboardFind:
                    return "KeyboardFind";
                case kHIDUsage_KeyboardMute:
                    return "KeyboardMute";
                case kHIDUsage_KeyboardVolumeUp:
                    return "KeyboardVolumeUp";
                case kHIDUsage_KeyboardVolumeDown:
                    return "KeyboardVolumeDown";
                case kHIDUsage_KeyboardLockingCapsLock:
                    return "KeyboardLockingCapsLock";
                case kHIDUsage_KeyboardLockingNumLock:
                    return "KeyboardLockingNumLock";
                case kHIDUsage_KeyboardLockingScrollLock:
                    return "KeyboardLockingScrollLock";
                case kHIDUsage_KeypadComma:
                    return "KeypadComma";
                case kHIDUsage_KeypadEqualSignAS400:
                    return "KeypadEqualSignAS400";
                case kHIDUsage_KeyboardInternational1:
                    return "KeyboardInternational1";
                case kHIDUsage_KeyboardInternational2:
                    return "KeyboardInternational2";
                case kHIDUsage_KeyboardInternational3:
                    return "KeyboardInternational3";
                case kHIDUsage_KeyboardInternational4:
                    return "KeyboardInternational4";
                case kHIDUsage_KeyboardInternational5:
                    return "KeyboardInternational5";
                case kHIDUsage_KeyboardInternational6:
                    return "KeyboardInternational6";
                case kHIDUsage_KeyboardInternational7:
                    return "KeyboardInternational7";
                case kHIDUsage_KeyboardInternational8:
                    return "KeyboardInternational8";
                case kHIDUsage_KeyboardInternational9:
                    return "KeyboardInternational9";
                case kHIDUsage_KeyboardLANG1:
                    return "KeyboardLANG1";
                case kHIDUsage_KeyboardLANG2:
                    return "KeyboardLANG2";
                case kHIDUsage_KeyboardLANG3:
                    return "KeyboardLANG3";
                case kHIDUsage_KeyboardLANG4:
                    return "KeyboardLANG4";
                case kHIDUsage_KeyboardLANG5:
                    return "KeyboardLANG5";
                case kHIDUsage_KeyboardLANG6:
                    return "KeyboardLANG6";
                case kHIDUsage_KeyboardLANG7:
                    return "KeyboardLANG7";
                case kHIDUsage_KeyboardLANG8:
                    return "KeyboardLANG8";
                case kHIDUsage_KeyboardLANG9:
                    return "KeyboardLANG9";
                case kHIDUsage_KeyboardAlternateErase:
                    return "KeyboardAlternateErase";
                case kHIDUsage_KeyboardSysReqOrAttention:
                    return "KeyboardSysReqOrAttention";
                case kHIDUsage_KeyboardCancel:
                    return "KeyboardCancel";
                case kHIDUsage_KeyboardClear:
                    return "KeyboardClear";
                case kHIDUsage_KeyboardPrior:
                    return "KeyboardPrior";
                case kHIDUsage_KeyboardReturn:
                    return "KeyboardReturn";
                case kHIDUsage_KeyboardSeparator:
                    return "KeyboardSeparator";
                case kHIDUsage_KeyboardOut:
                    return "KeyboardOut";
                case kHIDUsage_KeyboardOper:
                    return "KeyboardOper";
                case kHIDUsage_KeyboardClearOrAgain:
                    return "KeyboardClearOrAgain";
                case kHIDUsage_KeyboardCrSelOrProps:
                    return "KeyboardCrSelOrProps";
                case kHIDUsage_KeyboardExSel:
                    return "KeyboardExSel";
                case kHIDUsage_KeyboardLeftControl:
                    return "KeyboardLeftControl";
                case kHIDUsage_KeyboardLeftShift:
                    return "KeyboardLeftShift";
                case kHIDUsage_KeyboardLeftAlt:
                    return "KeyboardLeftAlt";
                case kHIDUsage_KeyboardLeftGUI:
                    return "KeyboardLeftGUI";
                case kHIDUsage_KeyboardRightControl:
                    return "KeyboardRightControl";
                case kHIDUsage_KeyboardRightShift:
                    return "KeyboardRightShift";
                case kHIDUsage_KeyboardRightAlt:
                    return "KeyboardRightAlt";
                case kHIDUsage_KeyboardRightGUI:
                    return "KeyboardRightGUI";
                default:
                    return format("Unknown (%|02X|)", usage);
            }
        case kHIDPage_Button:
            return format("Button %|d|", usage);
        case kHIDPage_LEDs:
        case kHIDPage_Simulation:
        case kHIDPage_VR:
        case kHIDPage_Sport:
        case kHIDPage_Game:
        case kHIDPage_GenericDeviceControls:
        case kHIDPage_Ordinal:
        case kHIDPage_Telephony:
        case kHIDPage_Consumer:
        case kHIDPage_Digitizer:
        case kHIDPage_PID:
        case kHIDPage_Unicode:
        case kHIDPage_AlphanumericDisplay:
        case kHIDPage_Sensor:
        case kHIDPage_Monitor:
        case kHIDPage_MonitorEnumerated:
        case kHIDPage_MonitorVirtual:
        case kHIDPage_MonitorReserved:
        case kHIDPage_PowerDevice:
        case kHIDPage_BatterySystem:
        case kHIDPage_PowerReserved:
        case kHIDPage_PowerReserved2:
        case kHIDPage_BarCodeScanner:
        case kHIDPage_Scale:
        case kHIDPage_MagneticStripeReader:
        case kHIDPage_CameraControl:
        case kHIDPage_Arcade:
        case kHIDPage_Undefined:
        default:
            return format("Unknown (0x%|02X|)", usage);
    }
}

std::string get_usage_page(IOHIDElementRef element) {
    auto page = IOHIDElementGetUsagePage(element);

    switch (page) {
        case kHIDPage_GenericDesktop:
            return "GenericDesktop";
        case kHIDPage_Simulation:
            return "Simulation";
        case kHIDPage_VR:
            return "VR";
        case kHIDPage_Sport:
            return "Sport";
        case kHIDPage_Game:
            return "Game";
        case kHIDPage_GenericDeviceControls:
            return "GenericDeviceControls";
        case kHIDPage_KeyboardOrKeypad:
            return "KeyboardOrKeypad";
        case kHIDPage_LEDs:
            return "LEDs";
        case kHIDPage_Button:
            return "Button";
        case kHIDPage_Ordinal:
            return "Ordinal";
        case kHIDPage_Telephony:
            return "Telephony";
        case kHIDPage_Consumer:
            return "Consumer";
        case kHIDPage_Digitizer:
            return "Digitizer";
        case kHIDPage_PID:
            return "PID";
        case kHIDPage_Unicode:
            return "Unicode";
        case kHIDPage_AlphanumericDisplay:
            return "AlphanumericDisplay";
        case kHIDPage_Sensor:
            return "Sensor";
        case kHIDPage_Monitor:
            return "Monitor";
        case kHIDPage_MonitorEnumerated:
            return "MonitorEnumerated";
        case kHIDPage_MonitorVirtual:
            return "MonitorVirtual";
        case kHIDPage_MonitorReserved:
            return "MonitorReserved";
        case kHIDPage_PowerDevice:
            return "PowerDevice";
        case kHIDPage_BatterySystem:
            return "BatterySystem";
        case kHIDPage_PowerReserved:
            return "PowerReserved";
        case kHIDPage_PowerReserved2:
            return "PowerReserved2";
        case kHIDPage_BarCodeScanner:
            return "BarCodeScanner";
        case kHIDPage_Scale:
            return "Scale";
        case kHIDPage_MagneticStripeReader:
            return "MagneticStripeReader";
        case kHIDPage_CameraControl:
            return "CameraControl";
        case kHIDPage_Arcade:
            return "Arcade";
        case kHIDPage_Undefined:
            return "Undefined";
        default:
            return format("Unknown page (0x%|02X|)", page);
    }
}

std::string describe_element(IOHIDElementRef element, int level, bool ignore) {
    auto indent = get_indent(level);

    std::string buffer{};
    CF::Array children = IOHIDElementGetChildren(element);

    buffer += format("%|s|element %|s| (%|d| children): page %|s|, usage %|s|",
        indent, get_element_type(element), children.GetCount(),
        get_usage_page(element), get_usage(element));

    if (ignore) {
        buffer += " (ignored)\n";
    } else {
        buffer += "\n";
        for (auto child : children) {
            buffer += describe_element((IOHIDElementRef)child, level + 1, false);
        }
    }

    return buffer;
}

const char* get_ioreturn_string(IOReturn value) {
    switch (value) {
        default:
            return nullptr;
        case kIOReturnSuccess:
            return "no error";
        case kIOReturnError:
            return "general error";
        case kIOReturnNoMemory:
            return "no memory";
        case kIOReturnNoResources:
            return "no resources";
        case kIOReturnIPCError:
            return "error during IPC";
        case kIOReturnNoDevice:
            return "no such device";
        case kIOReturnNotPrivileged:
            return "privilege violation";
        case kIOReturnBadArgument:
            return "invalid argument";
        case kIOReturnLockedRead:
            return "device read locked";
        case kIOReturnLockedWrite:
            return "device write locked";
        case kIOReturnExclusiveAccess:
            return "device already open for exclusive access";
        case kIOReturnBadMessageID:
            return "sent and received message had different ID";
        case kIOReturnUnsupported:
            return "unsupported function";
        case kIOReturnVMError:
            return "misc. VM failure";
        case kIOReturnInternalError:
            return "internal error";
        case kIOReturnIOError:
            return "general I/O error";
        case kIOReturnCannotLock:
            return "cannot acquire lock";
        case kIOReturnNotOpen:
            return "device not open";
        case kIOReturnNotReadable:
            return "read not supported";
        case kIOReturnNotWritable:
            return "write not supported";
        case kIOReturnNotAligned:
            return "alignment error";
        case kIOReturnBadMedia:
            return "media error";
        case kIOReturnStillOpen:
            return "device still open";
        case kIOReturnRLDError:
            return "rld failure";
        case kIOReturnDMAError:
            return "DMA failure";
        case kIOReturnBusy:
            return "device busy";
        case kIOReturnTimeout:
            return "I/O timeout";
        case kIOReturnOffline:
            return "device offline";
        case kIOReturnNotReady:
            return "not ready";
        case kIOReturnNotAttached:
            return "device not attached";
        case kIOReturnNoChannels:
            return "no DMA channels left";
        case kIOReturnNoSpace:
            return "no space for data";
        case kIOReturnPortExists:
            return "port already exists";
        case kIOReturnCannotWire:
            return "cannot wire down physical memory";
        case kIOReturnNoInterrupt:
            return "no interrupt attached";
        case kIOReturnNoFrames:
            return "no DMA frames enqueued";
        case kIOReturnMessageTooLarge:
            return "oversized message received on interrupt port";
        case kIOReturnNotPermitted:
            return "not permitted";
        case kIOReturnNoPower:
            return "no power to device";
        case kIOReturnNoMedia:
            return "media not present";
        case kIOReturnUnformattedMedia:
            return "media not formatted";
        case kIOReturnUnsupportedMode:
            return "no such mode";
        case kIOReturnUnderrun:
            return "data underrun";
        case kIOReturnOverrun:
            return "data overrun";
        case kIOReturnDeviceError:
            return "device not working properly";
        case kIOReturnNoCompletion:
            return "a completion routine is required";
        case kIOReturnAborted:
            return "operation aborted";
        case kIOReturnNoBandwidth:
            return "bus bandwidth would be exceeded";
        case kIOReturnNotResponding:
            return "device not responding";
        case kIOReturnIsoTooOld:
            return "I/O request too old";
        case kIOReturnIsoTooNew:
            return "I/O request too new";
        case kIOReturnNotFound:
            return "data not found";
        case kIOReturnInvalid:
            return "invalid return code";
    }
}

template <typename... Args>
void throw_osx_error(const std::string& fmt, Args&&... args) {
    auto message = format(fmt, std::forward<Args>(args)...);
    throw std::runtime_error(message);
}

template <typename... Args>
void throw_osx_error(IOReturn code, const std::string& fmt, Args&&... args) {
    auto message = format(fmt, std::forward<Args>(args)...);
    auto code_message = get_ioreturn_string(code);

    if (code_message != nullptr) {
        message = format("%1%: %2%", message, code_message);
    } else {
        message = format("%|s|: unknown error code %|x|", message, code);
    }

    throw std::runtime_error(message);
}

std::ofstream m_log;

template <typename... Args>
void do_log(std::string level, std::string message, Args&&... args) {
    message = format(message, std::forward<Args>(args)...);
    m_log << format("[%s] %s", level, message) << std::endl;
}

template <typename... Args>
void log(std::string message, Args&&... args) {
    do_log("INFO", message, std::forward<Args>(args)...);
}

template <typename... Args>
void log_err(std::string message, Args&&... args) {
    do_log("ERROR", message, std::forward<Args>(args)...);
}

CF::Dictionary match_usage(uint32_t usage_page, uint32_t usage) {
    CF::Dictionary dict{};
    dict << CF::Pair{ kIOHIDDeviceUsagePageKey, CF::Number{ usage } };
    dict << CF::Pair{ kIOHIDDeviceUsageKey, CF::Number{ usage_page } };
    return dict;
}

IOHIDManagerRef m_ref;

void open_manager() {
    auto code = IOHIDManagerOpen(m_ref, kIOHIDManagerOptionNone);
    if (code != kIOReturnSuccess) {
        throw_osx_error(code, "Failed to open HIDManager");
    }
}

void add_device(IOHIDDeviceRef device) {
    std::string name = CF::String{ IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductKey)) };
    int location = CF::Number{ IOHIDDeviceGetProperty(device, CFSTR(kIOHIDLocationIDKey)) };
    int unique = CF::Number{ IOHIDDeviceGetProperty(device, CFSTR(kIOHIDUniqueIDKey)) };
    int vendor = CF::Number{ IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVendorIDKey)) };
    int product = CF::Number{ IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductIDKey)) };
    int version = CF::Number{ IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVersionNumberKey)) };
    std::string serial = CF::String{ IOHIDDeviceGetProperty(device, CFSTR(kIOHIDSerialNumberKey)) };

    std::string sep = "======================================================";
    std::string report = "Device found:\n" + sep + "\n";

    report += format("Name:      %s\nLocation:  %s\nUniqueID:  %s\nVendorID:  %s\nProductID: %s\nVersion:   %s\nSerial:    %s\n\n",
        name, location, unique, vendor, product, version, serial);

    CF::Array top_elements = IOHIDDeviceGetProperty(device, CFSTR(kIOHIDElementKey));
    std::string description{};
    int ignored{};

    for (auto element_dict : top_elements) {
        auto cookie = (CFTypeRef)CFDictionaryGetValue((CFDictionaryRef)element_dict, CFSTR(kIOHIDElementCookieKey));

        CF::Dictionary match{};
        match << CF::Pair{ kIOHIDElementCookieKey, cookie };

        auto match_result_ref = IOHIDDeviceCopyMatchingElements(device, match, 0);
        CF::Array match_result = match_result_ref;
        CFRelease(match_result_ref);

        auto element = (IOHIDElementRef)match_result[0];
        auto ignore_me = true;

        if (IOHIDElementGetType(element) == kIOHIDElementTypeCollection) {
            if (IOHIDElementGetCollectionType(element) == kIOHIDElementCollectionTypeApplication) {
                //auto page = IOHIDElementGetUsagePage(element);
                //auto usage = IOHIDElementGetUsage(element);
                ignore_me = false;
            }
        }

        description += describe_element(element, 1, ignore_me);
        if (ignore_me) {
            ignored++;
        }
    }

    report += format("%|d| top-level elements, %|d| ignored:\n", top_elements.GetCount(), ignored) + description;
    report += "\n" + sep + "\n\n";

    log(report);
}

void make_report() {
    m_ref = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDManagerOptionNone);

    log("Enumerating devices");
    IOHIDManagerSetDeviceMatchingMultiple(m_ref, nullptr);
    open_manager();

    auto device_set = IOHIDManagerCopyDevices(m_ref);
    if (device_set == nullptr) {
        log("No devices found");
    } else {
        auto device_count = CFSetGetCount(device_set);
        log("%1% devices found", device_count);

        std::vector<IOHIDDeviceRef> devices{};
        devices.resize(device_count);
        CFSetGetValues(device_set, (const void**)&devices[0]);
        CFRelease(device_set);

        for (auto&& device : devices) {
            add_device(device);
        }
    }
}

int main() {
    m_log = std::ofstream{"osx-hid-report.txt"};

    try {
        make_report();
    } catch (const std::exception& e) {
        log_err("Unhandled exception: %s", e.what());
    }

    m_log.flush();
    m_log.close();
}
