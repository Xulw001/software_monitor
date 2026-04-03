#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <ntdef.h>

#include "krtl/string.h"

inline void CopyUnicodeString(rtl::wstring& dst, PCUNICODE_STRING src) {
    __try {
        if (src != nullptr && src->Buffer != nullptr) {
            dst = rtl::move(
                rtl::wstring(src->Buffer, src->Length / sizeof(wchar_t)));
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        ;
    }
}

#endif
