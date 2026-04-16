#include "proc_util.h"

#include <ntifs.h>

#include "string_util.h"

bool GetCurrentProcessImageName(rtl::wstring& full_image_name) {
    PEPROCESS ob_proc = nullptr;
    if (!NT_SUCCESS(
            PsLookupProcessByProcessId(PsGetCurrentProcessId(), &ob_proc))) {
        return false;
    }

    PUNICODE_STRING proc_image_name = NULL;
    NTSTATUS status = SeLocateProcessImageName(ob_proc, &proc_image_name);
    if (NT_SUCCESS(status)) {
        if (proc_image_name->Length) {
            CopyUnicodeString(full_image_name, proc_image_name);
        }
        ExFreePool(proc_image_name);
    }
    ObDereferenceObject(ob_proc);
    return true;
}
