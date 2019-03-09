; ---------------------
;       x64.nsh
; ---------------------
;
; A few simple macros to handle installations on x64 machines.
;
; RunningX64 checks if the installer is running on x64.
;
;   ${If} ${RunningX64}
;     MessageBox MB_OK "running on x64"
;   ${EndIf}
;
; DisableX64FSRedirection disables file system redirection.
; EnableX64FSRedirection enables file system redirection.
;
;   SetOutPath $SYSDIR
;   ${DisableX64FSRedirection}
;   File some.dll # extracts to C:\Windows\System32
;   ${EnableX64FSRedirection}
;   File some.dll # extracts to C:\Windows\SysWOW64
;

!ifndef ___X64__NSH___
!define ___X64__NSH___

!include LogicLib.nsh

!macro _RunningX64 _a _b _t _f
  !insertmacro _LOGICLIB_TEMP
  System::Call kernel32::GetCurrentProcess()i.s
  System::Call kernel32::IsWow64Process(is,*i.s)
  Pop $_LOGICLIB_TEMP
  !insertmacro _!= $_LOGICLIB_TEMP 0 `${_t}` `${_f}`
!macroend

!define RunningX64 `"" RunningX64 ""`

!macro DisableX64FSRedirection

  System::Call kernel32::Wow64EnableWow64FsRedirection(i0)

!macroend

!define DisableX64FSRedirection "!insertmacro DisableX64FSRedirection"

!macro EnableX64FSRedirection

  System::Call kernel32::Wow64EnableWow64FsRedirection(i1)

!macroend

!define EnableX64FSRedirection "!insertmacro EnableX64FSRedirection"

!endif # !___X64__NSH___
