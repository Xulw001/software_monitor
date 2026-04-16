#ifndef IOCTL_H
#define IOCTL_H

#include <Windows.h>

#include <atomic>
#include <thread>
#include <vector>

namespace ioctl {

struct OverlappedInfo {
    OVERLAPPED overlapped;
    std::unique_ptr<uint8_t[]> buffer;
};

class Communication {
   public:
    static Communication& GetInstance() {
        static Communication instance;
        return instance;
    }

    bool Start();
    void Stop();

   private:
    void DoSingleRequest(OverlappedInfo* overlapped_info);
    void DoSingleResponse(OverlappedInfo* overlapped_info);

    void DoCompleteRequest();

   private:
    HANDLE file_ = INVALID_HANDLE_VALUE;
    HANDLE complete_port_ = nullptr;
    std::vector<std::thread> thread_pool_;
    std::vector<OverlappedInfo> overlapped_infos_;
    std::atomic<bool> is_stopped_ = false;
};

}  // namespace ioctl

#endif