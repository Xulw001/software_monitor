#include "dev.h"

#include "ioctl.h"
#include "shared.h"

namespace monitor {
namespace dev {

PDEVICE_OBJECT device_ = nullptr;

_Function_class_(DRIVER_DISPATCH)
    _IRQL_requires_max_(DISPATCH_LEVEL) _IRQL_requires_same_ static NTSTATUS
    IrpDispatcher(PDEVICE_OBJECT, PIRP irp) {
    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION pisl = IoGetCurrentIrpStackLocation(irp);
    if (pisl->MajorFunction == IRP_MJ_DEVICE_CONTROL) {
        switch (pisl->Parameters.DeviceIoControl.IoControlCode) {
            case IOCTL_FETCH_DATA:
                return ioctl::Dispatcher(irp);
            default:
                status = STATUS_INVALID_DEVICE_REQUEST;
                break;
        }
    }

    irp->IoStatus.Status = status;
    irp->IoStatus.Information = 0;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return status;
}

bool initialize(PDRIVER_OBJECT drv) {
    UNICODE_STRING dev_name;
    RtlInitUnicodeString(&dev_name, kDeviceName);
    if (!NT_SUCCESS(IoCreateDevice(drv, 0, &dev_name, FILE_DEVICE_UNKNOWN,
                                   FILE_DEVICE_SECURE_OPEN, FALSE, &device_)))
        return false;
    device_->Flags |= DO_BUFFERED_IO;

    UNICODE_STRING dev_sym_name;
    RtlInitUnicodeString(&dev_sym_name, kSymbolName);
    if (!NT_SUCCESS(IoCreateSymbolicLink(&dev_sym_name, &dev_name)))
        return false;

    for (auto i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
        drv->MajorFunction[i] = IrpDispatcher;
    }
    return true;
}

void finalize() {
    UNICODE_STRING dev_sym_name;
    RtlInitUnicodeString(&dev_sym_name, kSymbolName);
    if (!NT_SUCCESS(IoDeleteSymbolicLink(&dev_sym_name))) {
        KdPrint(("IoDeleteSymbolicLink failed\n"));
    }

    if (device_) {
        IoDeleteDevice(device_);
        device_ = nullptr;
    }
}
}  // namespace dev
}  // namespace monitor