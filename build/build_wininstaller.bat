@echo off

echo ============================
echo Set version %1
echo ============================
python set_version.py %1 > ./../src/version.h
python set_version.py %1 > ./../3rdparty/crashreporter/src/version.h
echo DONE

echo ============================
echo Setup Build Environment
echo ============================
cd ./../
set SRCDIR=%cd%
set QTDIR=D:\Qt\5.3\msvc2012_opengl\bin\
if not defined DevEnvDir (
call "C:\\Program Files (x86)\\Microsoft Visual Studio 11.0\\VC\\vcvarsall.bat"
)
echo DONE

echo ============================
echo Build project
echo ============================
echo Build Crash Reporter :
cd ./3rdparty/crashreporter
%QTDIR%/qmake -v
%QTDIR%/qmake CONFIG+=release DESTDIR=%SRCDIR%/bin/windows/release
nmake /NOLOGO /S clean >nul: 2>nul:
nmake /NOLOGO /S

if %errorlevel% neq 0 (
 echo !!! Crashreporter Compilation Error !!!
 exit /b %errorlevel%
)

cd %SRCDIR%
echo ============================
echo Build Application :
cd ./src
%QTDIR%/qmake -v
%QTDIR%/qmake CONFIG+=release
nmake /NOLOGO /S clean >nul: 2>nul:
nmake /NOLOGO /S

if %errorlevel% neq 0 (
 echo !!! Compilation Error !!!
 exit /b %errorlevel%
)

cd ./../

echo ============================
echo Build installer
echo ============================
"C:\\Program Files (x86)\\NSIS\\Unicode\\makensis.exe" /V1 /DVERSION=%1  ./build/windows/installer/installer.nsi

echo ============================
echo Copy installer
echo ============================
del /F /Q .\\bin\\*.exe
cp ./build/windows/installer/redis-desktop-manager-%1.exe .\\bin\\redis-desktop-manager-%1.exe
