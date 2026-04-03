#ifndef PROC_UTIL_H
#define PROC_UTIL_H

#include "krtl/string.h"

bool GetCurrentProcessImageName(rtl::wstring& full_image_name);

#endif