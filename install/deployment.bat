@echo off

if "%1"=="-install" (
    call :install
    exit /b
)

if "%1"=="-uninstall" (
    call :uninstall
    exit /b
)

echo usage: deployment.bat [-install][-uninstall]
exit /b

:uninstall
sc stop InstallMonitorService
sc delete InstallMonitorService
sc delete InstallMonitor
exit /b

:install
call :register_drv
sc create InstallMonitorService binPath="%~dp0\install_monitor_srv.exe" type=own
sc start InstallMonitorService
exit /b

:register_drv
sc create InstallMonitor binPath="%~dp0\install_monitor_drv.sys" type=filesys group="FSFilter Activity Monitor" depend="FltMgr"

reg add "HKLM\SYSTEM\CurrentControlSet\Services\InstallMonitor\Instances" /f
reg add "HKLM\SYSTEM\CurrentControlSet\Services\InstallMonitor\Instances" /v DefaultInstance /t REG_SZ /d "InstallMonitor Instance" /f
reg add "HKLM\SYSTEM\CurrentControlSet\Services\InstallMonitor\Instances\InstallMonitor Instance" /f
reg add "HKLM\SYSTEM\CurrentControlSet\Services\InstallMonitor\Instances\InstallMonitor Instance" /v Altitude /t REG_SZ /d "389850" /f
reg add "HKLM\SYSTEM\CurrentControlSet\Services\InstallMonitor\Instances\InstallMonitor Instance" /v Flags /t REG_DWORD /d 0 /f
exit /b
