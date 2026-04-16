#include "ioctl.h"

#include "safe_list.h"

namespace monitor {
namespace ioctl {

const size_t kMaxDataListSize = 1024;

enum class IrpState { kPending, kComplete, kReset };

static const size_t kMaxIrpListSize = 1024;

struct IOCTLContext {
    PDEVICE_OBJECT device = nullptr;
    SafeList irp_list;
    SafeList data_list;

   public:
    IOCTLContext(DataClean irp_clean, DataClean data_clean)
        : irp_list(irp_clean), data_list(data_clean) {}
};

IOCTLContext* ioctl_ctx_ = nullptr;

static inline void CompleteIrp(PIRP irp, NTSTATUS status,
                               ULONG_PTR information) {
    irp->IoStatus.Status = status;
    irp->IoStatus.Information = information;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
}

static inline void CleanupPendingIrp(PVOID buffer) {
    if (!buffer) return;
    CompleteIrp((PIRP)buffer, STATUS_CANCELLED, 0);
}

static inline void CleanupPendingData(PVOID buffer) {
    if (!buffer) return;
    delete[] (PUINT8)buffer;
}

static IrpState CompleteIrpWithPendingData(PIRP irp, MessageHeader* msg) {
    PIO_STACK_LOCATION pisl = IoGetCurrentIrpStackLocation(irp);
    if (pisl->Parameters.DeviceIoControl.OutputBufferLength < msg->size) {
        KdPrint(("output buffer length is too small\n"));
        CompleteIrp(irp, STATUS_BUFFER_TOO_SMALL, msg->size);
        return IrpState::kReset;
    }

    RtlCopyMemory(irp->AssociatedIrp.SystemBuffer, msg + 1, msg->size);
    CompleteIrp(irp, STATUS_SUCCESS, msg->size);
    return IrpState::kComplete;
}

_Function_class_(DRIVER_CANCEL) _Requires_lock_held_(_Global_cancel_spin_lock_)
    _Releases_lock_(_Global_cancel_spin_lock_)
        _IRQL_requires_min_(DISPATCH_LEVEL)
            _IRQL_requires_(DISPATCH_LEVEL) static VOID
    NotifyIrpCancelRoutine(PDEVICE_OBJECT, PIRP irp) {
    IoReleaseCancelSpinLock(irp->CancelIrql);
    if (!ioctl_ctx_ || !ioctl_ctx_->irp_list.Pop(irp)) return;
    CleanupPendingIrp(irp);
}

static IrpState CompleteIrpRoutine(PIRP irp) {
    if (!ioctl_ctx_) {
        CompleteIrp(irp, STATUS_DEVICE_NOT_READY, 0);
        return IrpState::kComplete;
    }

    auto msg = (MessageHeader*)ioctl_ctx_->data_list.Pop();
    if (msg) {
        if (CompleteIrpWithPendingData(irp, msg) == IrpState::kComplete) {
            CleanupPendingData(msg);
        } else if (!ioctl_ctx_->data_list.Push(msg)) {
            CleanupPendingData(msg);
        }
        return IrpState::kComplete;
    }

    return IrpState::kPending;
}

static void PendingIrpRoutine(PIRP irp) {
    IoMarkIrpPending(irp);

    KIRQL irql;
    IoAcquireCancelSpinLock(&irql);
    if (irp->Cancel) {
        IoReleaseCancelSpinLock(irql);
        CompleteIrp(irp, STATUS_CANCELLED, 0);
        return;
    }

    IoSetCancelRoutine(irp, NotifyIrpCancelRoutine);

    if (!ioctl_ctx_->irp_list.Push(irp)) {
        IoSetCancelRoutine(irp, nullptr);
        IoReleaseCancelSpinLock(irql);
        CompleteIrp(irp, STATUS_NO_MEMORY, 0);
        return;
    }

    IoReleaseCancelSpinLock(irql);
    return;
}

NTSTATUS Dispatcher(PIRP irp) {
    if (CompleteIrpRoutine(irp) == IrpState::kComplete) {
        return irp->IoStatus.Status;
    }
    PendingIrpRoutine(irp);
    return STATUS_PENDING;
}

bool SendMessage(MessageHeader* msg) {
    if (!ioctl_ctx_) {
        KdPrint(("ioctl is uninitialized\n"));
        return false;
    }

    auto irp = (PIRP)ioctl_ctx_->irp_list.Pop();
    if (irp) {
        if (IoSetCancelRoutine(irp, nullptr) == nullptr) {
            CleanupPendingIrp(irp);
        } else if (CompleteIrpWithPendingData(irp, msg) ==
                   IrpState::kComplete) {
            CleanupPendingData(msg);
            return true;
        }
    }

    if (ioctl_ctx_->data_list.Size() >= kMaxDataListSize) {
        auto data = (MessageHeader*)ioctl_ctx_->data_list.Pop();
        if (data) CleanupPendingData(data);
    }

    if (!ioctl_ctx_->data_list.Push(msg)) {
        KdPrint(("no memory to push to data list\n"));
        return false;
    }
    return true;
}

bool initialize() {
    ioctl_ctx_ = new (PoolTag::NonPagedNx)
        IOCTLContext(CleanupPendingIrp, CleanupPendingData);
    if (!ioctl_ctx_) return false;
    return true;
}

void finalize() {
    if (!ioctl_ctx_) return;

    delete ioctl_ctx_;
    ioctl_ctx_ = nullptr;
}
}  // namespace ioctl
}  // namespace monitor
