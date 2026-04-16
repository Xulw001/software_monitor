#include "ioctl.h"

#include "install_monitor.h"
#include "log_util.h"
#include "shared.h"

namespace ioctl {

const DWORD kMaxMsgSize = 4096;
const int kMaxThreadNum = 4;

bool Communication::Start() {
    do {
        file_ =
            CreateFileW(kSymbolName, GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                        OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
        if (file_ == INVALID_HANDLE_VALUE) {
            LOG_ERR << "open device failed at " << GetLastError();
            break;
        }

        complete_port_ = CreateIoCompletionPort(file_, nullptr, 0, 0);
        if (complete_port_ == nullptr) {
            LOG_ERR << "create complete port failed at " << GetLastError();
            break;
        }

        overlapped_infos_.resize(kMaxThreadNum);
        for (auto& overlapped_info : overlapped_infos_) {
            overlapped_info.buffer.reset(new uint8_t[kMaxMsgSize]);
            DoSingleRequest(&overlapped_info);
        }

        thread_pool_.resize(kMaxThreadNum);
        for (auto& thread : thread_pool_) {
            thread = std::thread(&Communication::DoCompleteRequest, this);
        }

        return true;
    } while (false);
    return false;
}

void Communication::Stop() {
    is_stopped_ = true;

    if (complete_port_) {
        CloseHandle(complete_port_);
        complete_port_ = nullptr;
    }

    for (auto& thread : thread_pool_) {
        if (thread.joinable()) thread.join();
    }

    if (file_ != INVALID_HANDLE_VALUE) {
        CloseHandle(file_);
        file_ = INVALID_HANDLE_VALUE;
    }
}

void Communication::DoCompleteRequest() {
    while (!is_stopped_) {
        DWORD msg_size = 0;
        ULONG_PTR completion_key = 0;
        LPOVERLAPPED overlapped = nullptr;
        if (!GetQueuedCompletionStatus(complete_port_, &msg_size,
                                       &completion_key, &overlapped,
                                       INFINITE)) {
            auto ec = GetLastError();
            if (ec == ERROR_ABANDONED_WAIT_0) {
                LOG_INF << "complete port abandoned";
            } else if (ec == ERROR_INSUFFICIENT_BUFFER) {
                LOG_ERR << "msg buffer is too small!";
                continue;
            } else {
                LOG_ERR << "get queued completion status failed at " << ec;
            }
            break;
        }

        DoSingleResponse((OverlappedInfo*)overlapped);
        DoSingleRequest((OverlappedInfo*)overlapped);
    }
    LOG_INF << "ioctl thread stopped!";
}

void Communication::DoSingleRequest(OverlappedInfo* overlapped_info) {
    ZeroMemory(&overlapped_info->overlapped, sizeof(OVERLAPPED));
    if (!DeviceIoControl(file_, IOCTL_FETCH_DATA, nullptr, 0,
                         overlapped_info->buffer.get(), kMaxMsgSize, nullptr,
                         &overlapped_info->overlapped)) {
        if (GetLastError() != ERROR_IO_PENDING) {
            LOG_ERR << "ioctl failed at " << GetLastError();
        }
    }
}

void Communication::DoSingleResponse(OverlappedInfo* overlapped_info) {
    InstallInfo* install_info = (InstallInfo*)overlapped_info->buffer.get();
    if (!install_info) return;

    std::wstring image((wchar_t*)install_info->buffer, install_info->image_len);
    std::wstring path((wchar_t*)install_info->buffer + install_info->image_len,
                      install_info->path_len);
    if (install_info->type == 0) {
        install::InstallMonitor::FileInstall(image, path);
    } else {
        install::InstallMonitor::RegistryInstall(image, path);
    }
}
}  // namespace ioctl