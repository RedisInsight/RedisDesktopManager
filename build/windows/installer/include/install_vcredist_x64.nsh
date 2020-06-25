!include LogicLib.nsh

!macro InstallVCredist
  !define VCplus_URL "https://aka.ms/vs/16/release/VC_redist.x64.exe"

  ReadRegDWORD $0 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" Bld
  ${If} $0 >= 27033
    goto VCInstalled
  ${Else}
    goto VCDownload
  ${EndIf}

  VCDownload:
  DetailPrint "Beginning download of VC++ 2015-2019 Redistributable."
  inetc::get /TIMEOUT=30000 ${VCplus_URL} "$TEMP\vc_redist.x64.exe" /END
  Pop $0
  DetailPrint "Result: $0"
  StrCmp $0 "OK" InstallVCplusplus
  StrCmp $0 "cancelled" VCCanceled
  inetc::get /TIMEOUT=30000 /NOPROXY ${VCplus_URL} "$TEMP\vc_redist.x64.exe" /END
  Pop $0
  DetailPrint "Result: $0"
  StrCmp $0 "OK" InstallVCplusplus

  MessageBox MB_ICONSTOP "Download failed: $0. Please install it manually and try again: $VCplus_URL"
  Abort

  InstallVCplusplus:
  DetailPrint "Completed download."
  Pop $0
  ${If} $0 == "cancel"
    MessageBox MB_YESNO|MB_ICONEXCLAMATION \
    "Download cancelled.  Continue Installation?" \
    IDYES VCInstalled IDNO VCCanceled
  ${EndIf}

  DetailPrint "Pausing installation while downloaded VC++ installer runs."
  DetailPrint "Installation could take several minutes to complete."
  ExecWait '$TEMP\vc_redist.x64.exe /passive /norestart'

  DetailPrint "Removing VC++ installer."
  Delete "$TEMP\vc_redist.x64.exe"

  DetailPrint "VC++ installer removed."
  goto VCInstalled

VCCanceled:
  Abort "Installation cancelled by user."

VCInstalled:
  Pop $0
!macroend
