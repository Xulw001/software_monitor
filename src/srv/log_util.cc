#include "log_util.h"

#include <Windows.h>
#include <pathcch.h>

#include <fstream>

const char* logger_cfg_path = ".\\logger.ini";
int log_level = LOG_INF_LEVEL;

static void UpdateCurrentDirectory() {
    wchar_t path[MAX_PATH] = {0};
    GetModuleFileNameW(NULL, path, MAX_PATH);
    PathCchRemoveFileSpec(path, MAX_PATH);
    SetCurrentDirectoryW(path);
}

void log_init() {
    UpdateCurrentDirectory();

    log_level = GetPrivateProfileIntA("logger", "level", LOG_INF_LEVEL,
                                      logger_cfg_path);

    char logger_type[36] = {0};
    GetPrivateProfileStringA("logger", "type", "console", logger_type,
                             sizeof(logger_type), logger_cfg_path);
    if (strcmp(logger_type, "file") == 0) {
        char log_file[MAX_PATH] = {0};
        GetPrivateProfileStringA("logger", "file",
                                 "install_monitor_service.log", log_file,
                                 sizeof(log_file), logger_cfg_path);
        Logger::instance().init(
            new std::ofstream(log_file, std::ios::out | std::ios::app));
    }
}
