$!
$!
$!
$ olddir = f$environment( "default" )
$ on control_y then goto End
$ on error then goto End
$!
$ gosub Init
$!
$ man2help sys$input: libssh2.hlp -b 1

LIBSSH2

OpenVMS port of the public domain libssh2 library, which 
provides an API to implement client SSH communciation.

License information is available at the copying subtopic.

$!
$ open/append mh libssh2.hlp
$ write mh helpversion
$ close mh
$!
$ man2help -a [-]readme.; libssh2.hlp        -b 2
$ man2help -a [-]authors.; libssh2.hlp       -b 2
$ man2help -a [-]copying.; libssh2.hlp       -b 2
$ man2help -a [-]news.; libssh2.hlp          -b 2
$ man2help -a [-]release-notes.; libssh2.hlp -b 2
$ man2help -a [-]hacking.; libssh2.hlp       -b 2
$ man2help -a [-]todo.; libssh2.hlp          -b 2
$!
$ man2help -a sys$input: libssh2.hlp         -b 2

API_Reference

Reference of all implemented API calls in
libssh2.

$!
$ man2help -a [-.docs]*.3 libssh2.hlp -b 3 -p
$!
$ library/help/create libssh2.hlb libssh2.hlp
$!
$End:
$ set default 'olddir'
$exit
$!
$!-------------------------------------------------------
$!
$Init:
$!
$ thisdir = f$environment( "procedure" )
$ thisdir = f$parse(thisdir,,,"device") + f$parse(thisdir,,,"directory")
$ set default 'thisdir'
$! 
$ say = "write sys$output"
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
$ helpversion = "This help library is based on build version ''majorv'.''minorv'.''patchv' of libssh2."
$!
$ deassign/job majorv
$ deassign/job minorv
$ deassign/job patchv
$!
$ if f$search( "man2help.exe" ) .eqs. ""
$ then
$   cc man2help
$   link man2help
$ endif
$! 
$ man2help := $'thisdir'man2help.exe 
$!
$ if f$search("libssh2.hlp") .nes. "" 
$ then 
$   delete libssh2.hlp;*
$ endif
$ if f$search("libssh2.hlb") .nes. "" 
$ then 
$   delete libssh2.hlb;*
$ endif
$return
