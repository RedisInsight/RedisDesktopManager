$!
$ olddir = f$environment("default")
$ on error then goto End
$!
$ gosub Init
$!
$ call WriteProductDescriptionFile
$ call WriteProductTextFile
$!
$! backup tree
$!
$ backup [-...]*.*;0/excl=([]*.exe,*.obj,*.opt,*.hlp,*.hlb,*.bck,*.com,*.pcsi*) -
    libssh2-'versionname''datename'_src.bck/save
$ purge libssh2-'versionname''datename'_src.bck
$!
$! backup examples
$!
$ backup [-.example]*.c;0 libssh2_examples-'versionname''datename'.bck/save
$ dire  libssh2_examples-'versionname''datename'.bck
$ purge libssh2_examples-'versionname''datename'.bck
$!
$ set default [-]
$!
$ defdir  = f$environment( "default" )
$ thisdev = f$parse(defdir,,,"device","no_conceal") 
$ thisdir = f$parse(defdir,,,"directory","no_conceal") - "][" - "][" - "][" - "]["
$!
$ libssh2_kf = thisdev + thisdir 
$ libssh2_kf = libssh2_kf - "]" + ".]"
$!
$ set default 'mdir'
$!
$ define/translation_attributes=concealed libssh2_kf 'libssh2_kf'
$!
$ product package libssh2 - 
    /base='arch' - 
    /producer=jcb -
    /source=[] - ! where to find PDF and PTF 
    /destination=[] - ! where to put .PCSI file 
    /material=libssh2_kf:[000000...] -      ! where to find product material 
    /version="''vms_majorv'.''minorv'-''patchv'''datename'" -
    /format=sequential 
$!
$End:
$!
$ set noon
$ if f$search("*.pcsi$desc;*") .nes. "" then delete *.pcsi$desc;*
$ if f$search("*.pcsi$text;*") .nes. "" then delete *.pcsi$text;*
$ if f$search("libssh2-''versionname'''datename'_src.bck;*") .nes. "" then delete libssh2-'versionname''datename'_src.bck;*
$ if f$search("libssh2_examples-''versionname'''datename'.bck;*") .nes. "" then delete libssh2_examples-'versionname''datename'.bck;*
$!
$ if f$trnlnm("libssh2_kf") .nes. "" then deassign libssh2_kf
$ set default 'olddir'
$!
$exit 
$!
$!--------------------------------------------------------------------------------
$!
$Init:
$ set process/parse=extended
$!
$ say    = "write sys$output"
$!
$ mdir   = f$environment("procedure") 
$ mdir   = mdir - f$parse(mdir,,,"name") - f$parse(mdir,,,"type") - f$parse(mdir,,,"version")
$!
$ set default 'mdir'
$!
$ pipe search [-.include]*.h libssh2_version_major/nohead | (read sys$input l ; l = f$element(2," ",f$edit(l,"trim,compress")) ; - 
       define/job majorv &l )
$ pipe search [-.include]*.h libssh2_version_minor/nohead | (read sys$input l ; l = f$element(2," ",f$edit(l,"trim,compress")) ; - 
       define/job minorv &l )
$ pipe search [-.include]*.h libssh2_version_patch/nohead | (read sys$input l ; l = f$element(2," ",f$edit(l,"trim,compress")) ; - 
       define/job patchv &l )
$!
$ majorv   = f$trnlnm("majorv")
$ minorv   = f$integer(f$trnlnm("minorv")) 
$ patchv   = f$integer( f$trnlnm("patchv"))
$!
$ deassign/job majorv
$ deassign/job minorv
$ deassign/job patchv
$!
$ vms_majorv =  f$trnlnm("vms_majorv")
$ if vms_majorv .eqs. "" then vms_majorv = majorv
$!
$ arch = "UNKNOWN"
$ if f$getsyi("arch_type") .eq. 2 then arch = "AXPVMS"
$ if f$getsyi("arch_type") .eq. 3 then arch = "I64VMS"
$!
$ if arch .eqs. "UNKNOWN"
$ then
$   say "Unsupported or unknown architecture, only works on Alpha and Itanium"
$   exit 2
$ endif
$!
$! is this a proper release or a daily snapshot?
$! crummy, but should work.
$!
$ daily = "TRUE"
$ firstdash     = f$locate("-",mdir)
$ restdir       = f$extract( firstdash + 1, 80, mdir)
$ seconddash    = f$locate("-", restdir)
$ if seconddash .ge. f$length( restdir )
$ then
$   daily    = "FALSE"
$   datename = "Final"
$ else
$   datename = "D" + f$extract(seconddash+1,8,restdir)   
$ endif
$!
$ if daily 
$ then
$   productname = "JCB ''arch' LIBSSH2 V''vms_majorv'.''minorv'-''patchv'''datename'"
$ else
$   productname = "JCB ''arch' LIBSSH2 V''vms_majorv'.''minorv'-''patchv'''datename'"
$ endif
$!
$ productfilename  = "JCB-''arch'-LIBSSH2-" + f$fao("V!2ZL!2ZL-!2ZL!AS-1", f$integer(vms_majorv),minorv,patchv,datename)
$!
$ versionname   = "''vms_majorv'_''minorv'_''patchv'"
$!
$return
$!
$!--------------------------------------------------------------------------------
$!
$WriteProductDescriptionFile: subroutine
$!
$ open/write pd 'productfilename'.PCSI$DESC
$!
$ write pd "product ''productname' full ;"
$ write pd "    software DEC ''arch' VMS ;"
$ write pd "    if (not <software DEC ''arch' VMS version minimum V8.3>) ;
$ write pd "        error NEED_VMS83 ;"
$ write pd "    end if ;"
$ write pd "    software HP ''arch' SSL version minimum V1.3;"
$ write pd "    if (not <software HP ''arch' SSL version minimum V1.3>) ;
$ write pd "        error NEED_SSL ;"
$ write pd "    end if ;"
$ write pd "    execute preconfigure (""set process/parse_type=extended"");"
$ write pd "    execute postinstall (""set process/parse_type=extended"","
$ write pd "    ""rename pcsi$destination:[gnv]usr.dir usr.DIR"","
$ write pd "    ""rename pcsi$destination:[gnv.usr]include.dir include.DIR"","
$ write pd "    ""rename pcsi$destination:[gnv.usr.include]libssh2.dir libssh2.DIR"","
$ write pd "    ""rename pcsi$destination:[gnv.usr.include.libssh2]libssh2.h libssh2.h"","
$ write pd "    ""rename pcsi$destination:[gnv.usr.include.libssh2]libssh2_publickey.h libssh2_publickey.h"","
$ write pd "    ""rename pcsi$destination:[gnv.usr.include.libssh2]libssh2_sftp.h libssh2_sftp.h"","
$ write pd "    ""rename pcsi$destination:[gnv.usr.include.libssh2]libssh2_config.h libssh2_config.h"","
$ write pd "    ""rename pcsi$destination:[gnv.usr]lib.dir lib.DIR"","
$ write pd "    ""rename pcsi$destination:[gnv.usr.lib]gnv$libssh2_''versionname'.exe gnv$libssh2_''versionname'.exe"","
$ write pd "    ""rename pcsi$destination:[gnv.usr.share.doc.libssh2]libssh2.hlb libssh2.hlb"");"
$ write pd "    information RELEASE_NOTES phase after ;"
$ write pd "    option EXAMPLE default 0 ;"
$ write pd "     directory ""[gnv.usr.share.doc.libssh2.examples]"" ;"
$ write pd "     file ""[gnv.usr.share.doc.libssh2.examples]libssh2_examples-''versionname'''datename'.bck"";"
$ write pd "    end option ;"
$ write pd "    option SOURCE default 0 ;"
$ write pd "     directory ""[gnv.common_src]"" ;"
$ write pd "     file ""[gnv.common_src]libssh2-''versionname'''datename'_src.bck"";"
$ write pd "    end option ;"
$ write pd "    directory ""[gnv]"" ;"
$ write pd "    directory ""[gnv.usr]"" ;"
$ write pd "    directory ""[gnv.usr.lib]"" ;"
$ write pd "    directory ""[gnv.usr.include]"" ;"
$ write pd "    directory ""[gnv.usr.include.libssh2]"" ;"
$ write pd "    directory ""[gnv.usr.share]"" ;"
$ write pd "    directory ""[gnv.usr.share.doc]"" ;"
$ write pd "    directory ""[gnv.usr.share.doc.libssh2]"" ;"
$ write pd "    file ""[gnv.usr.include.libssh2]libssh2.h"" source ""[include]libssh2.h"";"
$ write pd "    file ""[gnv.usr.include.libssh2]libssh2_publickey.h"" source ""[include]libssh2_publickey.h"";"
$ write pd "    file ""[gnv.usr.include.libssh2]libssh2_sftp.h"" source ""[include]libssh2_sftp.h"";"
$ write pd "    file ""[gnv.usr.include.libssh2]libssh2_config.h"" source ""[vms]libssh2_config.h"";"
$ write pd "    file ""[gnv.usr.share.doc.libssh2]libssh2.hlb"" source ""[vms]libssh2.hlb"";"
$ write pd "    file ""[gnv.usr.share.doc.libssh2]libssh2-''versionname'.news"" source ""[000000]NEWS."";"
$ write pd "    file ""[gnv.usr.share.doc.libssh2]libssh2-''versionname'.release_notes"" source ""[vms]readme.vms"";"
$ write pd "    file ""[gnv.usr.lib]gnv$libssh2_''versionname'.exe"" source ""[vms]libssh2_''versionname'.exe"";"
$ write pd "end product ;"
$ close pd
$exit 
$endsubroutine
$!
$!--------------------------------------------------------------------------------
$!
$WriteProductTextFile: subroutine
$!
$ open/write pt 'productfilename'.PCSI$TEXT
$ write pt "=PRODUCT ''productname' Full"
$ write pt "1 'PRODUCER"
$ write pt "=prompt libssh2 is an open source product ported to VMS by Jose Baars"
$ write pt "This software product is provided with no warranty."
$ write pt "For license information see the LIBSSH2 help library."
$ write pt "1 'PRODUCT"
$ write pt "=prompt JCB LIBSSH2 for OpenVMS"
$ write pt ""
$ write pt "libssh2 is an open source client side library that aims to implement"
$ write pt "the SSH protocol. This is the OpenVMS port of that library."
$ write pt "Further information at http://www.libssh2.org."
$ write pt ""
$ write pt "1 NEED_VMS83"
$ write pt "=prompt OpenVMS 8.3 or later is not installed on your system."
$ write pt "This product requires OpenVMS 8.3 or later to function."
$ write pt ""
$ write pt "1 NEED_SSL"
$ write pt "=prompt HP SSL 1.3 or later is not installed on your system."
$ write pt "This product requires HP SSL 1.3 or later to function."
$ write pt ""
$ write pt "1 RELEASE_NOTES"
$ write pt "=prompt Release notes and the libssh2 help library are available in [gnv.usr.share.doc.libssh2] directory."
$ write pt ""
$ write pt "1 EXAMPLE"
$ write pt "=prompt Do you want the libssh2 C programming examples ? "
$ write pt "The libssh2 coding examples will be available in backup saveset "
$ write pt "[gnv.usr.share.doc.libssh2.examples]libssh2_examples_''versionname'.bck"
$ write pt ""
$ write pt "1 SOURCE"
$ write pt "=prompt Do you want the complete libssh2 source tree ? "
$ write pt "The libssh2 source tree will be available in backup saveset "
$ write pt "[gnv.common_src]libssh2_''versionname'''datename'_src.bck"
$close pt
$exit
$ endsubroutine

