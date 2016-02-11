@echo off

if exist nuget.exe (
    echo Nuget found
) else (
    echo Download nuget cmd utility
    powershell -Command "(New-Object Net.WebClient).DownloadFile('https://nuget.org/nuget.exe', 'nuget.exe')"
)

echo Install deps with nuget
nuget install -Version 1.2.8.6 -OutputDirectory ./../3rdparty/windows rmt_zlib

echo Run qredisclient configure.bat
cd .\..\\3rdparty\\qredisclient\\src\\
configure.bat
cd ../../