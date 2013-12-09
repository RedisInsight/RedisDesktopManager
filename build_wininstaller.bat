@echo off

echo =======================================================================
echo Setup Compiler environment
echo =======================================================================

call "C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\vcvarsall.bat"

echo =======================================================================
echo Build project
echo =======================================================================
set msbuildemitsolution=1
msbuild RDM.sln /t:Rebuild /property:Configuration=Release

echo =======================================================================
echo Build installer
echo =======================================================================
"C:\\Program Files (x86)\\NSIS\\Unicode\\makensis.exe" /V1 /DVERSION=%1  ./build/windows/installer/installer.nsi

echo =======================================================================
echo Copy installer
echo =======================================================================
cp ./build/windows/installer/redis-desktop-manager-%1.exe .\\releases\\redis-desktop-manager-%1.exe
