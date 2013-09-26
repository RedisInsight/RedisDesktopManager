$!
$!
$ olddir = f$environment("default")
$ on control_y then goto YExit
$!
$ gosub Init
$ if .not. init_status then goto YExit
$!
$ call CompileAll
$ call BuildTransferVectors
$ call LinkShared
$!
$ call Cleanup
$!
$YExit:
$ set noon
$!
$ deassign srcdir
$ if f$search("objdir:*.*;*") .nes. "" then delete objdir:*.*;*
$ deassign objdir
$ delete library_objects.dir;*
$!
$ set default 'olddir'
$exit 
$!
$!---------------------------------------------------------------------
$!
$Init: 
$!
$!
$ init_status = 1
$ thisid = f$integer( %x'f$getjpi(0,"pid")') 
$ mdir   = f$environment("procedure") 
$ mdir   = mdir - f$parse(mdir,,,"name") - f$parse(mdir,,,"type") - f$parse(mdir,,,"version")
$ set default 'mdir'
$!
$ objdir = "[.library_objects]"
$ srcdir = "[-.src]"
$!
$ objdirfile = objdir - "[." - "]" + ".dir"
$ if f$search( objdirfile ) .eqs. ""
$ then
$   create/directory 'objdir'
$ endif
$!
$ define objdir 'objdir'
$ define srcdir 'srcdir'
$!
$ cc_include = "/include=([],[-.include])"
$ link_opts  = "objdir:libssh2_''thisid'.opt"
$!
$ pipe search [-.include]libssh2.h libssh2_version_major/nohead | (read sys$input l ; l = f$element(2," ",f$edit(l,"trim,compress")) ; - 
       define/job majorv &l )
$ pipe search [-.include]libssh2.h libssh2_version_minor/nohead | (read sys$input l ; l = f$element(2," ",f$edit(l,"trim,compress")) ; - 
       define/job minorv &l )
$ pipe search [-.include]libssh2.h libssh2_version_patch/nohead | (read sys$input l ; l = f$element(2," ",f$edit(l,"trim,compress")) ; - 
       define/job patchv &l )
$!
$ majorv   = f$trnlnm("majorv")
$ minorv   = f$integer(f$trnlnm("minorv")) 
$ patchv   = f$integer( f$trnlnm("patchv"))
$!
$ OLBONLY = "FALSE"
$ if p1 .eqs. "OLBONLY"
$ then
$   OLBONLY = "TRUE"
$ endif
$!
$ deassign/job majorv
$ deassign/job minorv
$ deassign/job patchv
$!
$return
$!
$!---------------------------------------------------------------------
$!
$Cleanup: subroutine
$!
$ set noon
$ purge *.opt
$ purge *.olb
$ purge *.exe
$! 
$exit 1
$endsubroutine
$!
$!---------------------------------------------------------------------
$!
$LinkShared: subroutine
$!
$!
$!
$ cversion = f$fao("!3ZL",minorv)  + f$fao("!3ZL",patchv)
$!
$! General linking options in link_libssh2_version...opt
$! Vectors in link_libssh2_vectors...opt
$!
$ open/write uitv link_libssh2_version_'majorv'_'minorv'_'patchv'.opt
$ write uitv "GSMATCH=LEQUAL,''majorv',''cversion'"
$ write uitv "IDENTIFICATION=""LIBSSH2 ''majorv'.''minorv'.''patchv'"""
$ write uitv "sys$share:ssl$libcrypto_shr32.exe/share"
$ write uitv "sys$share:ssl$libssl_shr32.exe/share"
$ write uitv "gnv$libzshr/share"
$ close uitv
$! 
$ link/shared/exe=libssh2_'majorv'_'minorv'_'patchv'.exe -
    libssh2.olb/lib, -
    link_libssh2_version_'majorv'_'minorv'_'patchv'.opt/opt, -
    link_libssh2_vectors_'majorv'_'minorv'_'patchv'.opt/opt
$!
$exit
$endsubroutine
$!
$!---------------------------------------------------------------------
$!
$CompileAll: subroutine
$!
$ set noon
$!
$ if f$search("objdir:*.obj;*") .nes "" 
$ then 
$   delete objdir:*.obj;*
$ endif
$ if f$search("[.cxx_repository]cxx$demangler_db.;") .nes "" 
$ then 
$   delete [.cxx_repository]cxx$demangler_db.;*
$ endif
$!
$! Compile all .c files in [-.src], first as_is
$! and then as default all uppercase names
$! and add the resulting object to object libraries
$! libssh2_up.olb and libssh2_as_is.olb.
$!
$ case = 0
$ if OLBONLY then case = 1
$CaseLoop:
$!
$ if case .eq. 0 
$ then!camel case names 
$   cc_flags = "/names=(shortened,as_is)"
$   objlib = "libssh2_asis.olb"
$ endif
$!
$ if case .eq. 1 
$ then!uppercase names 
$   if f$search("[.cxx_repository]cxx$demangler_db.;") .nes "" 
$   then 
$      rename [.cxx_repository]cxx$demangler_db.; *.lowercase
$      purge  [.cxx_repository]cxx$demangler_db.lowercase
$   endif
$!
$   cc_flags = "/names=(shortened)"
$   objlib = "libssh2_up.olb"
$ endif
$!
$ if f$search("''objlib';*") .nes. "" then delete 'objlib';*
$ library/create 'objlib'
$!
$Loop:
$ this = f$search("srcdir:*.c;0")
$ if this .eqs. "" then goto EndLoop
$!
$ what = f$parse( this,,,"name")
$!
$ call CompileAndAdd
$!
$ goto Loop
$EndLoop:
$ case = case + 1
$ delete objdir:*.obj;*
$ if case .lt 2 then goto CaseLoop
$!
$ rename libssh2_up.olb libssh2.olb
$ if f$search("[.cxx_repository]cxx$demangler_db.;") .nes "" 
$ then 
$    rename [.cxx_repository]cxx$demangler_db.; *.uppercase
$    purge  [.cxx_repository]cxx$demangler_db.uppercase
$ endif
$!
$ if OLBONLY then exit 4
$!
$! For each function that is too long, create a global symbol
$! low$'shortened-uppercase-name' with as value lowercase shortened
$! name in it, so we can add the proper lower or mixed case 
$! shortened name later when building the transfer vectors 
$! for the shared image.
$! This is to prevent two very long similar function names 
$! that are shortened getting mixed up when sorted alphabetically.
$!
$ inputfile = "[.cxx_repository]cxx$demangler_db.lowercase"
$ gosub GetShortened
$!
$ inputfile =  "[.cxx_repository]cxx$demangler_db.uppercase"
$ gosub GetShortened
$!
$exit
$!
$GetShortened:
$!
$ open/read s 'inputfile'
$ namecount = 0
$ReadLoop:
$!
$ read/end=endreadloop s regel
$!
$ shortname = f$element(0,"$",regel) + "$"
$ longname  = f$element(1,"$",regel)
$!
$ symvalue = ""
$!
$ if shortname .eqs. f$edit(shortname,"upcase")
$ then
$!  this is an uppercase shortname, add it 
$   symname  = "u$''longname'"
$   symvalue = "''shortname'"
$   low$'shortname' == l$'longname'
$!
$   delete/symbol l$'longname'
$!
$ else
$!  this is an lowercase shortname
$   symname  = "l$''longname'"
$   symvalue = "''shortname'"
$   'symname' = "''symvalue'"
$ endif
$!
$ namecount = namecount + 1
$!
$ goto ReadLoop
$EndReadLoop:
$!
$close s
$return
$!
$endsubroutine
$!
$!---------------------------------------------------------------------
$!
$CompileAndAdd: subroutine
$!
$ on error then goto End
$!
$ cc /warn=disable=longextern/lis=objdir:/show=all 'cc_include' 'cc_flags'/object=objdir:'what'.obj srcdir:'what'.c
$ library/insert 'objlib' objdir:'what'.obj
$!
$End:
$exit
$endsubroutine 
$!
$!---------------------------------------------------------------------
$!
$BuildTransferVectors: subroutine
$!
$! Do a balanced read of the uppercase library names
$! and the mixed case library names, and build the
$! transfer vectors with uppercase entry points 
$! with an alternative in mixed case.
$! For shortened names, use the low$* symbols
$! to avoid being fooled by the sort.
$!
$ thislib = "libssh2.olb"
$ library/lis=libu.'thisid'/names libssh2.olb
$ library/lis=lib_asisu.'thisid'/names libssh2_asis.olb
$!
$! case blind sort of all modules in both the uppercase
$! as the case sensitive object library.
$!
$ sort libu.'thisid' lib.'thisid'/spec=sys$input
/COLLATING_SEQUENCE=(SEQUENCE= ("A" - "Z","0"-"9","_"), FOLD) 
$ sort lib_asisu.'thisid' lib_asis.'thisid'/spec=sys$input
/COLLATING_SEQUENCE=(SEQUENCE= ("A" - "Z","0"-"9","_"), FOLD) 
$!
$ open/read  in  lib.'thisid'
$ open/read  inasis  lib_asis.'thisid'
$ open/write uit link_libssh2_vectors_'majorv'_'minorv'_'patchv'.opt
$!
$ write uit "CASE_SENSITIVE=YES"
$ write uit "SYMBOL_VECTOR= ( -"
$!
$ mode = 0
$ uitregel = ""
$!
$ReadLoop:
$!
$ read/end=ReadAsis in regel
$ReadAsis:
$ read/end=EndReadLoop inasis asisregel
$!
$ regel     = f$edit( regel, "trim,compress" )
$ asisregel = f$edit( asisregel, "trim,compress" )
$!
$ if f$element(0," ",regel) .eqs. "Module" .or. -
     f$extract(0,1,regel)   .eqs. "_" .or. -
     f$element(1," ",regel) .nes. " " .or. -
     regel .eqs. ""
$ then
$   goto ReadLoop
$ endif
$!
$ if uitregel .nes. "" .and. mode .eq. 1
$ then
$   write uit "''uitregel'=PROCEDURE, -" 
$   write uit "''uitasis'/''uitregel'=PROCEDURE, -" 
$!
$   uitregel = ""
$   uitasis  = ""
$ endif 
$!
$ uitregel = regel
$ if f$type( low$'uitregel' ) .nes. ""
$ then
$   uitasis = low$'uitregel'
$   delete/symbol/global low$'uitregel'
$ else
$   uitasis  = asisregel
$ endif
$!
$ mode = 1
$!
$ goto ReadLoop
$EndreadLoop:
$! 
$! To get the closing brace after the last procedure
$! keyword.
$!
$ if uitregel .nes. ""
$ then 
$   write uit "''uitregel'=PROCEDURE, -" 
$   write uit "''uitasis'/''uitregel'=PROCEDURE)" 
$ endif
$!
$ write uit "CASE_SENSITIVE=NO"
$!
$ close in
$ close inasis
$ close uit
$!
$ delete lib*.'thisid';*
$!
$exit
$endsubroutine
$!
$!---------------------------------------------------------------------
$!
