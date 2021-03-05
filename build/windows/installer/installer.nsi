!addincludedir .\include
!addplugindir .\plugin

Name "RDM"

BrandingText "Open source GUI management tool for Redis"

RequestExecutionLevel admin

SetCompress auto
SetCompressor /SOLID /FINAL lzma
ManifestDPIAware true

# General Symbol Definitions
!define REGKEY "SOFTWARE\$(Name)"
!define COMPANY "Igor Malinovskiy"
!define URL rdm.dev
!define APP_EXE "rdm.exe"

# MUI Symbol Definitions
!define MUI_ICON "..\..\..\src\resources\images\logo.ico"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN $INSTDIR\${APP_EXE}
!define MUI_UNICON "..\..\..\src\resources\images\logo.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP ".\images\main.bmp"

# Included files
!include "nsProcess.nsh"
!include "x64.nsh"
!include "install_vcredist_x64.nsh"
!include Sections.nsh
!include MUI2.nsh

# Variables
Var StartMenuGroup

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE ..\..\..\LICENSE
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES


# Installer languages
!insertmacro MUI_LANGUAGE English

# Installer attributes
OutFile rdm-${VERSION}.exe
InstallDir $PROGRAMFILES64\RDM
CRCCheck on
XPStyle on
ShowInstDetails show
VIProductVersion ${VERSION}.0
VIAddVersionKey /LANG=${LANG_ENGLISH} ProductName "RDM"
VIAddVersionKey /LANG=${LANG_ENGLISH} ProductVersion "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} CompanyName "${COMPANY}"
VIAddVersionKey /LANG=${LANG_ENGLISH} CompanyWebsite "${URL}"
VIAddVersionKey /LANG=${LANG_ENGLISH} FileVersion "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} FileDescription ""
VIAddVersionKey /LANG=${LANG_ENGLISH} LegalCopyright ""
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails show


# Installer sections
Section -Main SEC0000
    ${nsProcess::KillProcess} "${APP_EXE}" $R4

    ${IfNot} ${RunningX64}
        MessageBox MB_OK "Starting from version 2019.0.0, RDM doesn't support 32-bit Windows"
        Quit
    ${EndIf}

    IfFileExists $INSTDIR\uninstall.exe already_installed not_installed
    already_installed:
    CopyFiles /SILENT /FILESONLY "$INSTDIR\uninstall.exe" "$INSTDIR\uninstall_.exe"
    ExecWait '"$INSTDIR\uninstall_.exe" /S _?=$INSTDIR'
    Sleep 100
    Delete /REBOOTOK $INSTDIR\uninstall_.exe

    not_installed:
    SetOutPath $INSTDIR    
    File /r resources\*
    WriteRegStr HKLM "${REGKEY}\Components" Main 1
    !insertmacro InstallVCredist
    BringToFront
SectionEnd

Section -post SEC0001
    WriteRegStr HKLM "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
    SetOutPath $SMPROGRAMS\$StartMenuGroup
    
    CreateShortCut "$DESKTOP\RDM.lnk" "$INSTDIR\${APP_EXE}" ""
    
    IfSilent 0 +2
        Exec "$INSTDIR\${APP_EXE}"

    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\RDM.lnk" "$INSTDIR\${APP_EXE}"
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\$(^UninstallLink).lnk" $INSTDIR\uninstall.exe

    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\uninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\uninstall.exe
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
SectionEnd

# Macro for selecting uninstaller sections
!macro SELECT_UNSECTION SECTION_NAME UNSECTION_ID
    Push $R0
    ReadRegStr $R0 HKLM "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${UNSECTION_ID}
    !insertmacro SelectSection "${UNSECTION_ID}"
    GoTo done${UNSECTION_ID}
next${UNSECTION_ID}:
    !insertmacro UnselectSection "${UNSECTION_ID}"
done${UNSECTION_ID}:
    Pop $R0
!macroend

# Uninstaller sections
Section /o -un.Main UNSEC0000
    ${nsProcess::KillProcess} "${APP_EXE}" $R4
    Sleep 1000
    Delete /REBOOTOK $INSTDIR\*
    RmDir /REBOOTOK /r $INSTDIR\*
    DeleteRegValue HKLM "${REGKEY}\Components" Main
SectionEnd

Section -un.post UNSEC0001
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
    Delete /REBOOTOK "$DESKTOP\RDM.lnk"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\RDM.lnk"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\$(^UninstallLink).lnk"
    Delete /REBOOTOK $INSTDIR\uninstall.exe
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"
    RmDir /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    RmDir /REBOOTOK $INSTDIR
SectionEnd

# Installer functions
Function .onInit
    InitPluginsDir
    StrCpy $StartMenuGroup RDM
FunctionEnd

# Uninstaller functions
Function un.onInit
    SetAutoClose true
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    StrCpy $StartMenuGroup RDM
    !insertmacro SELECT_UNSECTION Main ${UNSEC0000}
FunctionEnd

# Installer Language Strings
LangString ^UninstallLink ${LANG_ENGLISH} "Uninstall $(^Name)"
