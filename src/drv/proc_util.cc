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
    SeLocateProcessImageName(ob_proc, &proc_image_name);
    if (proc_image_name && proc_image_name->Length) {
        CopyUnicodeString(full_image_name, proc_image_name);
    }
    ExFreePoolWithTag(proc_image_name, 0);
    ObDereferenceObject(ob_proc);
    return true;
}
