#include <ntifs.h>

#include "dev.h"
#include "file_monitor.h"
#include "install_monitor.h"
#include "ioctl.h"
#include "reg_monitor.h"

_Function_class_(DRIVER_UNLOAD) static VOID finalize(_In_ PDRIVER_OBJECT) {
    monitor::reg::finalize();
    monitor::file::finalize();
    monitor::install::finalize();
    monitor::ioctl::finalize();
    monitor::dev::finalize();
}

_Function_class_(DRIVER_INITIALIZE) _IRQL_requires_same_
    _IRQL_requires_(PASSIVE_LEVEL)
EXTERN_C NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT drv, _In_ PUNICODE_STRING) {
    do {
        if (!monitor::dev::initialize(drv)) {
            break;
        }
        if (!monitor::ioctl::initialize()) {
            break;
        }
        if (!monitor::install::initialize()) {
            break;
        }
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