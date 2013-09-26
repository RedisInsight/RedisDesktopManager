$!
$!
$ olddir = f$environment("default")
$ on control_y then goto YExit
$!
$ gosub Init
$ if .not. init_status then goto YExit
$!
$ if what .eqs. "ALL"
$ then
$	call MakeAll
$ else
$	call Make 
$endif
$!
$YExit:
$ set noon
$! deassign exadir
$! deassign objdir
$ delete 'link_opts';*
$ set default 'olddir'
$exit 

$MakeAll: subroutine
$!
$ set noon
$Loop:
$ this = f$search("exadir:*.c;0")
$ if this .eqs. "" then goto EndLoop
$!
$ what = f$parse( this,,,"name")
$ call Make
$!
$ goto Loop
$EndLoop:
$!
$exit
$endsubroutine


$Make: subroutine
$!
$ set noon
$!
$ cc 'cc_include' 'cc_flags'/object=objdir:'what' exadir:'what'
$ sev = $severity
$ if sev .and. 2 
$ then
$	say "Error compiling ''what', not linked."
$ else
$ 	if .not. (sev .and. 1)  
$ 	then
$		say "Compile warnings in  ''what'"
$ 	endif
$  link/exe='what'.exe objdir:'what'.obj, 'link_opts'/opt
$ endif
$!
$!
$End:
$ delete objdir:'what'.obj;*
$exit
$endsubroutine 


$Init: 
$!
$!
$ init_status = 1
$ thisid = f$integer( %x'f$getjpi(0,"pid")') + "''f$cvtime(,,"second")'"
$ mdir   = f$environment("procedure") 
$ mdir   = mdir - f$parse(mdir,,,"name") - f$parse(mdir,,,"type") - f$parse(mdir,,,"version")
$ set default 'mdir'
$!
$ objdir = "[.example_objects]"
$ exadir = "[-.example]"
$!
$ objdirfile = objdir - "[." - "]" + ".dir"
$ if f$search( objdirfile ) .eqs. ""
$ then
$	create/directory 'objdir'
$ endif
$!
$ define objdir 'objdir'
$ define exadir 'exadir'
$!
$ cc_include = "/include=([],[-.include])"
$ cc_flags   = "/name=shortened/show=all"
$ link_opts  = "objdir:libssh2_''thisid'.opt"
$!
$!
$ what = "''p1'" 
$ if what .eqs. "" .or. f$edit(p1,"trim,collapse,upcase") .eqs. "ALL" 
$ then 
$	what = "ALL"
$ else
$	what = f$parse(what,,,"name")
$	if f$search("exadir:''what'.c") .eqs. ""
$	then 
$		write sys$output "Can't make ''what'"
$		init_status = 0
$	endif
$ endif
$!
$ currentlib = f$search("libssh2*.exe")
$!
$ define libssh2 'currentlib'
$!
$ how  = "''p2'"
$ if how .eqs. "" .or. f$edit(p2,"trim,collapse,upcase") .eqs. "STATIC" 
$ then
$ 	open/write lout 'link_opts'
$ 	write lout "libssh2.olb/lib"
$ 	write lout "sys$share:ssl$libcrypto_shr32.exe/share"
$ 	write lout "sys$share:ssl$libssl_shr32.exe/share"
$ 	write lout "gnv$libzshr/share"
$ 	close lout
$ else
$	how = "SHARED"
$ 	open/write lout 'link_opts'
$ 	write lout "libssh2/share"
$ 	close lout
$ endif 
$!
$return
