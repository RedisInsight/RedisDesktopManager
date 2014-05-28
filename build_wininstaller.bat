@echo off

set QTDIR=D:\Qt\5.3\msvc2012_opengl\bin\

echo =======================================================================
echo Setup Compiler environment
echo =======================================================================
if not defined DevEnvDir (
call "C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\vcvarsall.bat"
)

echo =======================================================================
echo Build project
echo =======================================================================
echo Build Crash Reporter :
cd ./crashreporter
%QTDIR%/qmake -v
%QTDIR%/qmake CONFIG+=release
nmake clean
nmake

cd ./../

echo Build Application :
cd ./redis-desktop-manager
%QTDIR%/qmake -v
%QTDIR%/qmake CONFIG+=release
nmake clean
nmake

cd ./../

echo =======================================================================
echo Build installer
echo =======================================================================
"C:\\Program Files (x86)\\NSIS\\Unicode\\makensis.exe" /V1 /DVERSION=%1  ./build/windows/installer/installer.nsi

echo =======================================================================
echo Copy installer
echo =======================================================================
del /F /Q .\\releases\\*.exe
cp ./build/windows/installer/redis-desktop-manager-%1.exe .\\releases\\redis-desktop-manager-%1.exe
