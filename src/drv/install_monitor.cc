#include "install_monitor.h"

#include "ioctl.h"
#include "safe_list.h"
#include "shared.h"

namespace monitor {
namespace install {

bool initialize() { return true; }

void finalize() {}

static ioctl::MessageHeader* BuildInstallInfo(UINT8 type,
                                              const rtl::wstring& image,
                                              const rtl::wstring& path) {
    size_t total_size = sizeof(ioctl::MessageHeader) + sizeof(InstallInfo) +
                        (image.size() + path.size()) * 2;
    auto buffer = new (PoolTag::NonPagedNx) UINT8[total_size];
    if (!buffer) return nullptr;

    ioctl::MessageHeader* message_buff = (ioctl::MessageHeader*)buffer;
    message_buff->size = total_size - sizeof(ioctl::MessageHeader);

    InstallInfo* install_info =
        (InstallInfo*)(buffer + sizeof(ioctl::MessageHeader));
    install_info->image_len = (UINT16)image.size();
    install_info->path_len = (UINT16)path.size();
    install_info->type = type;
    wcsncpy((wchar_t*)install_info->buffer, image.data(), image.size());
    wcsncpy((wchar_t*)install_info->buffer + image.size(), path.data(),
            path.size());
    return message_buff;
}

void FileInstall(const rtl::wstring& image, const rtl::wstring& path) {
    auto message_buff = BuildInstallInfo(0, image, path);
    if (!message_buff) return;
    if (!ioctl::SendMessage(message_buff)) delete[] (PUINT8)message_buff;
}

void RegistryInstall(const rtl::wstring& image, const rtl::wstring& path) {
    auto message_buff = BuildInstallInfo(1, image, path);
    if (!message_buff) return;
    if (!ioctl::SendMessage(message_buff)) delete[] (PUINT8)message_buff;
}
}  // namespace install
}  // namespace monitor