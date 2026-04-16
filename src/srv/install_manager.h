#ifndef INSTALL_MANAGER_H
#define INSTALL_MANAGER_H

#include <string>

namespace install {

struct Software {
    std::string product;
    std::string company;
    std::string version;
};

bool InsertSoftware(const std::wstring& process_path, int& software_id);
bool InsertAsset(const std::wstring& path, int software_id, int type);

}  // namespace install
#endif
