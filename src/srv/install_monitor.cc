#include "install_monitor.h"

#include <Windows.h>

#include "install_manager.h"
#include "log_util.h"

namespace install {

void InstallMonitor::FileInstall(const std::wstring& image,
                                 const std::wstring& path) {
    std::wstring standard_process_path;
    std::wstring target_file_path;
    if (!ConvertFilePath(image, standard_process_path)) {
        return;
    }
    if (!ConvertFilePath(path, target_file_path)) {
        return;
    }
    int software_id = 0;
    if (!InsertSoftware(standard_process_path, software_id)) {
        return;
    }
    if (!InsertAsset(target_file_path, software_id, 0)) {
        return;
    }
}

void InstallMonitor::RegistryInstall(const std::wstring& image,
                                     const std::wstring& path) {
    std::wstring standard_reg_path;
    std::wstring standard_process_path;
    if (!ConvertFilePath(image, standard_process_path)) {
        return;
    }
    if (!ConvertRegistryPath(path, standard_reg_path)) {
        return;
    }
    int software_id = 0;
    if (!InsertSoftware(standard_process_path, software_id)) {
        return;
    }
    if (!InsertAsset(standard_reg_path, software_id, 1)) {
        return;
    }
}

bool InstallMonitor::ConvertFilePath(const std::wstring& path,
                                     std::wstring& target_path) {
    auto drive_mask = GetLogicalDrives();
    if (!drive_mask) {
        LOG_ERR << "get logical drives failed at " << GetLastError();
        return false;
    }

    wchar_t logical_driver[] = L"X:";
    wchar_t device_path[MAX_PATH];
    for (auto i = 0; i < 26; i++) {
        if (drive_mask & (1 << i)) {
            logical_driver[0] = i + L'A';
            if (!QueryDosDeviceW(logical_driver, device_path, MAX_PATH)) {
                continue;
            }
            auto device_len = wcslen(device_path);
            if (wcsnicmp(device_path, path.c_str(), device_len) == 0) {
                target_path = logical_driver + path.substr(device_len);
                return true;
            }
        }
    }
    return false;
}

bool InstallMonitor::ConvertRegistryPath(const std::wstring& path,
                                         std::wstring& target_path) {
    const std::wstring hklm_prefix = L"\\REGISTRY\\MACHINE\\";
    const std::wstring hku_prefix = L"\\REGISTRY\\USER\\";
    if (path.compare(0, hklm_prefix.size(), hklm_prefix) == 0) {
        target_path = L"HKEY_LOCAL_MACHINE\\" + path.substr(hklm_prefix.size());
        return true;
    }
    if (path.compare(0, hku_prefix.size(), hku_prefix) == 0) {
        target_path = L"HKEY_CURRENT_USER\\" + path.substr(hku_prefix.size());
        return true;
    }
    return false;
}

}  // namespace install