#ifndef INSTALL_MONITOR_H
#define INSTALL_MONITOR_H

#include "string_util.h"

namespace monitor {
namespace install {

bool initialize();
void finalize();

void FileInstall(const rtl::wstring& image, const rtl::wstring& path);
void RegistryInstall(const rtl::wstring& image, const rtl::wstring& path);

}  // namespace install
}  // namespace monitor
#endif