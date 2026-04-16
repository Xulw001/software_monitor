#include <windows.h>

#include "ioctl.h"
#include "log_util.h"
#include "sql_helper.h"

#define SERVICE_NAME L"InstallMonitorService"
#define SERVICE_DRIVER_NAME L"InstallMonitor"

HANDLE service_stop_event = nullptr;
SERVICE_STATUS_HANDLE service_status_handle = nullptr;

SERVICE_STATUS service_status = {
    SERVICE_WIN32_OWN_PROCESS, SERVICE_STOPPED, 0, NO_ERROR, 0, 0, 10000};

bool LoadDriver() {
    auto scm_handle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (scm_handle == NULL) {
        LOG_ERR << "open scm failed at " << GetLastError();
        return false;
    }

    auto service_handle =
        OpenServiceW(scm_handle, SERVICE_DRIVER_NAME, SERVICE_ALL_ACCESS);
    if (service_handle == NULL) {
        LOG_ERR << "open service failed at " << GetLastError();
        CloseServiceHandle(scm_handle);
        return false;
    }

    BOOL status = StartServiceW(service_handle, 0, NULL);
    if (!status) {
        if (GetLastError() == ERROR_SERVICE_ALREADY_RUNNING) {
            LOG_INF << "driver already loaded!";
            status = true;
        } else {
            LOG_ERR << "start service failed at " << GetLastError();
        }
    } else {
        LOG_INF << "driver loaded successfully!";
    }

    CloseServiceHandle(service_handle);
    CloseServiceHandle(scm_handle);

    return status;
}

bool UnloadDriver() {
    auto scm_handle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (scm_handle == NULL) {
        LOG_ERR << "open scm failed at " << GetLastError();
        return false;
    }

    auto service_handle =
        OpenServiceW(scm_handle, SERVICE_DRIVER_NAME, SERVICE_ALL_ACCESS);
    if (service_handle == NULL) {
        LOG_ERR << "open service failed at " << GetLastError();
        CloseServiceHandle(scm_handle);
        return false;
    }

    SERVICE_STATUS status;
    if (!ControlService(service_handle, SERVICE_CONTROL_STOP, &status)) {
        LOG_ERR << "stop service failed at " << GetLastError();
        return false;
    }

    LOG_INF << "driver unloaded successfully!";

    CloseServiceHandle(service_handle);
    CloseServiceHandle(scm_handle);
    return true;
}

VOID WINAPI ServiceCtrlHandler(DWORD ctrlCode) {
    switch (ctrlCode) {
        case SERVICE_CONTROL_STOP:
        case SERVICE_CONTROL_SHUTDOWN:
            SetServiceStatus(service_status_handle, &service_status);
            SetEvent(service_stop_event);
            break;
    }
}

void ServiceMain(DWORD argc, LPWSTR* argv) {
    service_status_handle =
        RegisterServiceCtrlHandlerW(SERVICE_NAME, ServiceCtrlHandler);
    if (service_status_handle == NULL) return;

    do {
        service_status.dwCurrentState = SERVICE_START_PENDING;
        SetServiceStatus(service_status_handle, &service_status);
        LOG_INF << "service start pending...";

        service_stop_event = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (service_stop_event == NULL) break;

        service_status.dwCurrentState = SERVICE_RUNNING;
        service_status.dwControlsAccepted =
            SERVICE_CONTROL_STOP | SERVICE_CONTROL_SHUTDOWN;
        if (!SetServiceStatus(service_status_handle, &service_status)) break;

        if (!SqlHelper::instance().Connect()) {
            LOG_ERR << "connect db failed!";
            break;
        }

        if (!LoadDriver()) {
            LOG_ERR << "load driver failed!";
            break;
        }

        if (ioctl::Communication::GetInstance().Start()) {
            LOG_INF << "service running!!!";

            WaitForSingleObjectEx(service_stop_event, INFINITE, TRUE);
            LOG_INF << "service is stopping...";

            ioctl::Communication::GetInstance().Stop();
        } else {
            LOG_ERR << "start communication failed!";
        }

        UnloadDriver();

    } while (false);
    LOG_INF << "service is stopped!!!";

    if (service_stop_event) CloseHandle(service_stop_event);

    service_status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(service_status_handle, &service_status);
    return;
}

int wmain() {
    log_init();

    const SERVICE_TABLE_ENTRYW service_table[] = {{SERVICE_NAME, ServiceMain},
                                                  {NULL, NULL}};

    if (!StartServiceCtrlDispatcherW(service_table)) {
        LOG_ERR << "StartServiceCtrlDispatcherW failed at " << GetLastError();
        return 1;
    }
    return 0;
}
