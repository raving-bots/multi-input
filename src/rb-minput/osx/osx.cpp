// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include "osx/osx.hpp"

namespace multi_input {
    namespace osx {
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
    }
}
