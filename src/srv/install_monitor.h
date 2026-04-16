#ifndef INSTALL_MONITOR_H
#define INSTALL_MONITOR_H

#include <string>

namespace install {

class InstallMonitor {
   public:
    static void FileInstall(const std::wstring& image,
                            const std::wstring& path);
    static void RegistryInstall(const std::wstring& image,
                                const std::wstring& path);

   private:
    static bool ConvertFilePath(const std::wstring& path,
                                std::wstring& target_path);
    static bool ConvertRegistryPath(const std::wstring& path,
                                    std::wstring& target_path);
};
}  // namespace install

#endif
