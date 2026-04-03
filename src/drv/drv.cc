#include <ntifs.h>

#include "file_monitor.h"
#include "reg_monitor.h"

_Function_class_(DRIVER_UNLOAD) static VOID finalize(_In_ PDRIVER_OBJECT) {
    monitor::reg::finalize();
    monitor::file::finalize();
}

_Function_class_(DRIVER_INITIALIZE) _IRQL_requires_same_
    _IRQL_requires_(PASSIVE_LEVEL)
EXTERN_C NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT drv, _In_ PUNICODE_STRING) {
    do {
        if (!monitor::file::initialize(drv)) {
            break;
        }
        if (!monitor::reg::initialize(drv)) {
            break;
        }
        drv->DriverUnload = finalize;
        return STATUS_SUCCESS;
    } while (false);
    finalize(drv);
    return STATUS_UNSUCCESSFUL;
}