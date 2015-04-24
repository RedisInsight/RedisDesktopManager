@echo off
taskkill /f /im rdm.exe
cd build/windows/installer/resources/
START /b rdm.exe
TIMEOUT 30
tasklist /FI "IMAGENAME eq rdm.exe" 2>NUL | find /I /N "rdm.exe">NUL

if "%ERRORLEVEL%"=="0" (
taskkill /f /im rdm.exe
exit 0
) ELSE (exit 1)
