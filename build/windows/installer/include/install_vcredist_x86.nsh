!include LogicLib.nsh

!macro InstallVCredist
  !define VCplus_URL "https://download.microsoft.com/download/9/3/F/93FCF1E7-E6A4-478B-96E7-D4B285925B00/vc_redist.x86.exe"
 
  ReadRegStr $0 HKLM "Software\Microsoft\DevDiv\vc\Servicing\14.0\RuntimeMinimum" Install
  StrCmp $0 "1" VCInstalled
  
  DetailPrint "Beginning download of VC++ 14 Redistributable."
  inetc::get /TIMEOUT=30000 ${VCplus_URL} "$TEMP\vc_redist.x86.exe" /END
  Pop $0
  DetailPrint "Result: $0"
  StrCmp $0 "OK" InstallVCplusplus
  StrCmp $0 "cancelled" VCCanceled
  inetc::get /TIMEOUT=30000 /NOPROXY ${VCplus_URL} "$TEMP\vc_redist.x86.exe" /END
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
  ExecWait '$TEMP\vc_redist.x86.exe /passive /norestart'

  DetailPrint "Removing VC++ installer."
  Delete "$TEMP\vc_redist.x86.exe"
  
  DetailPrint "VC++ installer removed."
  goto VCInstalled
 
VCCanceled:
  Abort "Installation cancelled by user."
 
VCInstalled:
  Pop $0
!macroend