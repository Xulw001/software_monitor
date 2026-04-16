#include "install_manager.h"

#include <Windows.h>

#include <vector>

#include "log_util.h"
#include "sql_helper.h"

namespace install {

std::string WcharToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";

    int length = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0,
                                     nullptr, nullptr);
    if (length <= 0) return "";

    std::string result(length, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], length,
                        nullptr, nullptr);
    result.pop_back();
    return result;
}

std::wstring Utf8ToWchar(const std::string& str) {
    if (str.empty()) return L"";

    int length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    if (length <= 0) return L"";

    std::wstring result(length, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], length);

    result.pop_back();
    return result;
}

static bool QueryVersionInfo(LPCVOID buffer, LPCWSTR key, std::string& value,
                             WORD& lang, WORD& code_page) {
    static const DWORD kFallbacks[] = {
        0x040904b0,  // en-US + Unicode
        0x040904e4,  // en-US + Windows Multilingual
        0x080404b0,  // zh-CN + Unicode
        0x000004b0,  // language-neutral + Unicode
        0x000004e4,  // language-neutral + Windows Multilingual
    };

    UINT info_size = 0;
    wchar_t* pValue = nullptr;

    wchar_t sub_block[MAX_PATH] = {0};
    for (auto i = 0; i <= sizeof(kFallbacks) / sizeof(kFallbacks[0]); i++) {
        if (i) {
            lang = HIWORD(kFallbacks[i - 1]);
            code_page = LOWORD(kFallbacks[i - 1]);
        }

        wsprintfW(sub_block, L"\\StringFileInfo\\%04x%04x\\", lang, code_page);
        wcscpy(sub_block + 25, key);

        if (VerQueryValueW(buffer, sub_block, (LPVOID*)&pValue, &info_size)) {
            value = WcharToUtf8(pValue);
            return true;
        }
    }
    return false;
}

bool GetFileFullVersionInfo(const std::wstring& filePath, Software& info) {
    auto file_ver_size = GetFileVersionInfoSizeW(filePath.c_str(), nullptr);
    if (!file_ver_size) {
        LOG_ERR << "GetFileVersionInfoSizeW failed, error: " << GetLastError();
        return false;
    }

    std::vector<BYTE> buffer(file_ver_size);
    if (!GetFileVersionInfoW(filePath.c_str(), 0, file_ver_size,
                             buffer.data())) {
        LOG_ERR << "GetFileVersionInfoW failed, error: " << GetLastError();
        return false;
    }

    UINT info_size = 0;
    WORD lang = 0, code_page = 0;
    WORD* translation = nullptr;
    if (VerQueryValueW(buffer.data(), L"\\VarFileInfo\\Translation",
                       (LPVOID*)&translation, &info_size)) {
        lang = translation[0];
        code_page = translation[1];
    }

    // fetch product name
    if (QueryVersionInfo(buffer.data(), L"ProductName", info.product, lang,
                         code_page)) {
        LOG_DBG << "Product name: " << info.product;
    }

    // fetch company name
    if (QueryVersionInfo(buffer.data(), L"CompanyName", info.company, lang,
                         code_page)) {
        LOG_DBG << "Company name: " << info.company;
    }

    // fetch product version
    if (QueryVersionInfo(buffer.data(), L"ProductVersion", info.version, lang,
                         code_page)) {
        LOG_DBG << "Product version: " << info.version;
    } else {
        if (QueryVersionInfo(buffer.data(), L"FileVersion", info.version, lang,
                             code_page)) {
            LOG_DBG << "File version: " << info.version;
        }
    }

    return true;
}

bool InsertSoftware(const std::wstring& process_path, int& software_id) {
    std::string process_path_utf8 = WcharToUtf8(process_path);
    if (!SqlHelper::instance().SelectSoftwareIdByPath(process_path_utf8,
                                                      software_id)) {
        LOG_ERR << "search software id failed at " << process_path_utf8 << "!";
        return false;
    }

    // if software id is not 0, then it is already inserted
    if (software_id) {
        return true;
    }

    Software info;
    if (!GetFileFullVersionInfo(process_path, info)) {
        LOG_ERR << "search version info failed at " << process_path_utf8 << "!";
        return false;
    }

    if (info.company.compare("Microsoft Corporation") == 0) {
        if (info.product.empty()) return false;
        // if product name is Microsoft Office, then it is a valid software
        else if (info.product.compare("Microsoft Office") == 0)
            ;
        else
            return false;
    }
    LOG_INF << "product name: " << info.product;

    // insert software into database
    if (!SqlHelper::instance().InsertSoftware(info.product, info.company,
                                              info.version, software_id)) {
        LOG_ERR << "insert software failed at " << process_path_utf8 << "!";
        return false;
    }

    // insert asset into database
    if (!SqlHelper::instance().InsertAsset(process_path_utf8, software_id, 0)) {
        LOG_ERR << "insert asset failed!";
        return false;
    }
    return true;
}

bool InsertAsset(const std::wstring& path, int software_id, int type) {
    if (!SqlHelper::instance().InsertAsset(WcharToUtf8(path), software_id,
                                           type)) {
        LOG_ERR << "insert asset failed!";
        return false;
    }
    return true;
}

}  // namespace install
