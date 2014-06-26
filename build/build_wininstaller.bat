@echo off

cd ./../
set SRCDIR=%cd%
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
cd ./3rdparty/crashreporter
%QTDIR%/qmake -v
%QTDIR%/qmake CONFIG+=release DESTDIR=%SRCDIR%/bin/windows/release DEFINES+=RDM_VERSION="\\\"%1\\\""
nmake clean
nmake


cd %SRCDIR%

echo Build Application :
cd ./src
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
del /F /Q .\\bin\\*.exe
cp ./build/windows/installer/redis-desktop-manager-%1.exe .\\bin\\redis-desktop-manager-%1.exe
