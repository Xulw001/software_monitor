#include "reg_monitor.h"

#include "proc_util.h"
#include "string_util.h"

namespace monitor {
namespace reg {

LARGE_INTEGER callback_cookie_;

LPCWSTR kAltitudeValue = L"389850";

static bool GetAbsoluteRegistryPath(PVOID object, rtl::wstring& reg_path) {
    if (object == nullptr) return false;

    UNICODE_STRING routine_name;
    RtlInitUnicodeString(&routine_name, L"CmCallbackGetKeyObjectIDEx");

    PCUNICODE_STRING path = nullptr;
    if (nullptr != MmGetSystemRoutineAddress(&routine_name)) {
        if (!NT_SUCCESS(CmCallbackGetKeyObjectIDEx(&callback_cookie_, object,
                                                   nullptr, &path, 0))) {
            return false;
        }
        CopyUnicodeString(reg_path, path);
        CmCallbackReleaseKeyObjectIDEx(path);
    } else {
        if (!NT_SUCCESS(CmCallbackGetKeyObjectID(&callback_cookie_, object,
                                                 nullptr, &path))) {
            return false;
        }
        CopyUnicodeString(reg_path, path);
    }

    return true;
}

_IRQL_requires_same_ _Function_class_(EX_CALLBACK_FUNCTION) static NTSTATUS
    NotifyRoutine(PVOID, PVOID arg1, PVOID arg2) {
    auto notify_type = (REG_NOTIFY_CLASS)(ULONG_PTR)arg1;
    PREG_POST_OPERATION_INFORMATION info =
        (PREG_POST_OPERATION_INFORMATION)arg2;
    if (info != nullptr) {
        switch (notify_type) {
            case RegNtPostCreateKeyEx: {
                if (info->PreInformation == nullptr ||
                    *((PREG_CREATE_KEY_INFORMATION_V1)info->PreInformation)
                            ->Disposition == REG_OPENED_EXISTING_KEY) {
                    break;
                }
            }
            case RegNtPostSetValueKey:
            case RegNtPostRenameKey: {
                if (info->Status == STATUS_SUCCESS && info->Object != nullptr) {
                    rtl::wstring reg_path;
                    if (!GetAbsoluteRegistryPath(info->Object, reg_path)) {
                        break;
                    }

                    rtl::wstring proc_image_name;
                    if (!GetCurrentProcessImageName(proc_image_name)) {
                        break;
                    }
                }
            } break;
        }
    }

    return STATUS_SUCCESS;
}

bool initialize(PDRIVER_OBJECT drv) {
    UNICODE_STRING usitude = {};
    RtlInitUnicodeString(&usitude, kAltitudeValue);
    if (!NT_SUCCESS(CmRegisterCallbackEx(NotifyRoutine, &usitude, drv, nullptr,
                                         &callback_cookie_, nullptr))) {
        return false;
    }
    return true;
}

void finalize() { CmUnRegisterCallback(callback_cookie_); }

}  // namespace reg
}  // namespace monitor