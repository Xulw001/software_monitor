#ifndef SAFE_LIST_H
#define SAFE_LIST_H

#include <wdm.h>

#include "krtl/new.h"
#include "spin_lock.h"

typedef void (*DataClean)(PVOID);

class SafeList {
   private:
    struct SafeListNode {
        LIST_ENTRY list;
        PVOID buffer;
    };

   public:
    SafeList(DataClean clean) : clean_(clean), size_(0) {
        ASSERT(clean != nullptr);
        InitializeListHead(&list_);
    }

    ~SafeList() { Clear(); }

    SafeList(const SafeList&) = delete;
    SafeList& operator=(const SafeList&) = delete;

    bool Push(PVOID buffer) {
        SafeListNode* node = new (PoolTag::NonPagedNx) SafeListNode();
        if (!node) return false;
        node->buffer = buffer;
        lock_.Lock();
        InsertTailList(&list_, &node->list);
        size_++;
        lock_.Unlock();
        return true;
    }

    PVOID Pop() {
        PLIST_ENTRY entry = nullptr;
        lock_.Lock();
        if (!IsListEmpty(&list_)) {
            entry = RemoveHeadList(&list_);
            size_--;
        }
        lock_.Unlock();
        return entry ? DeleteNode(CONTAINING_RECORD(entry, SafeListNode, list))
                     : nullptr;
    }

    PVOID Pop(PVOID buffer) {
        SafeListNode* node = nullptr;
        lock_.Lock();
        for (auto entry = list_.Flink; entry != &list_; entry = entry->Flink) {
            node = CONTAINING_RECORD(entry, SafeListNode, list);
            if (node->buffer == buffer) {
                RemoveEntryList(entry);
                size_--;
                break;
            }
        }
        lock_.Unlock();
        return DeleteNode(node);
    }

    void Clear() {
        RTL_STATIC_LIST_HEAD(list);
        lock_.Lock();
        if (!IsListEmpty(&list_)) {
            list.Flink = list_.Flink;
            list.Blink = list_.Blink;
            list_.Flink->Blink = &list;
            list_.Blink->Flink = &list;
            InitializeListHead(&list_);
        }
        size_ = 0;
        lock_.Unlock();

        while (!IsListEmpty(&list)) {
            auto entry = RemoveHeadList(&list);
            clean_(DeleteNode(CONTAINING_RECORD(entry, SafeListNode, list)));
        }
    }

    INT32 Size() { return size_; }

   private:
    PVOID DeleteNode(SafeListNode* node) {
        PVOID buffer = nullptr;
        if (node != nullptr) {
            buffer = node->buffer;
            delete node;
        }
        return buffer;
    }

   private:
    SpinLock lock_;
    DataClean clean_;
    volatile INT32 size_;
    LIST_ENTRY list_;
};

#endif