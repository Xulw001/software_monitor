#ifndef SHARED_H
#define SHARED_H

#ifdef WIN32
#include <Windows.h>
#else
#include <wdm.h>
#endif

static constexpr LPCWSTR kDeviceName = L"\\Device\\InstallMonitor0743";
static constexpr LPCWSTR kSymbolName = L"\\??\\InstallMonitor0743";

#define IOCTL_FETCH_DATA \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS)

struct InstallInfo {
    UINT16 image_len;
    UINT16 path_len;
    UINT8 type;
    char buffer[1];
};

#endif
