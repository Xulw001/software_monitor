#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <wdm.h>

class SpinLock {
   public:
    SpinLock() { KeInitializeSpinLock(&lock_); }

    _Acquires_exclusive_lock_(lock_) void Lock() {
        KeAcquireSpinLock(&lock_, &irql_);
    }

    _Requires_exclusive_lock_held_(lock_) void Unlock() {
        KeReleaseSpinLock(&lock_, irql_);
    }

    ~SpinLock() {}

   private:
    KSPIN_LOCK lock_;
    KIRQL irql_ = PASSIVE_LEVEL;
};

#endif
