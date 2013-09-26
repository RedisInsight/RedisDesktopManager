#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <starlet.h>
#include <lib$routines.h>
#include <ssdef.h>
#include <descrip.h>
#include <rms.h>

typedef struct manl{
    struct manl *next;
    char *filename;
}man, *manPtr;
 
typedef struct pf_fabnam{
    struct FAB dfab;
    struct RAB drab;
    struct namldef dnam;
    char   expanded_filename[NAM$C_MAXRSS + 1]; 
} pfn, *pfnPtr;

/*----------------------------------------------------------*/

fpcopy( char *output, char *input, int len )
{
char    *is, *os;
int i;

if ( len ){
    for ( is = input, os = output, i = 0; i < len ; ++i, ++is, ++os){
            *os = *is;
    }
    *os = 0;
}else{
    output[0] = 0;
}
}           


/*----------------------------------------------------------*/
/* give part of ilename in partname. See code for proper
   value of i ( 0 = node, 1 = dev, 2 = dir,3 = name etc.
*/ 

int fnamepart( char *inputfile, char *part, int whatpart )
{
pfnPtr pf;
int     status;
char    ipart[6][256], *i, *p;

pf = calloc( 1, sizeof( pfn ) );

pf->dfab = cc$rms_fab;
pf->drab = cc$rms_rab;
pf->dnam = cc$rms_naml;

pf->dfab.fab$l_naml = &pf->dnam;

pf->dfab.fab$l_fna = (char *) -1; 
pf->dfab.fab$l_dna = (char *) -1; 
pf->dfab.fab$b_fns = 0;
pf->dfab.fab$w_ifi = 0;

pf->dnam.naml$l_long_defname = NULL; //inputfile;
pf->dnam.naml$l_long_defname_size = 0;//strlen( inputfile );

pf->dnam.naml$l_long_filename = inputfile;
pf->dnam.naml$l_long_filename_size = strlen( inputfile);

pf->dnam.naml$l_long_expand = pf->expanded_filename;
pf->dnam.naml$l_long_expand_alloc = NAM$C_MAXRSS ;

pf->dnam.naml$b_nop |= NAML$M_SYNCHK | NAML$M_PWD;

status = sys$parse( &pf->dfab, 0,0);
if ( !(status&1) ){
    free( pf );
    return( status );
}

fpcopy ( ipart[0], pf->dnam.naml$l_long_node , pf->dnam.naml$l_long_node_size);
fpcopy ( ipart[1], pf->dnam.naml$l_long_dev , pf->dnam.naml$l_long_dev_size);
fpcopy ( ipart[2], pf->dnam.naml$l_long_dir , pf->dnam.naml$l_long_dir_size);
fpcopy ( ipart[3], pf->dnam.naml$l_long_name , pf->dnam.naml$l_long_name_size);
fpcopy ( ipart[4], pf->dnam.naml$l_long_type , pf->dnam.naml$l_long_type_size);                                               
fpcopy ( ipart[5], pf->dnam.naml$l_long_ver , pf->dnam.naml$l_long_ver_size);

for( i = ipart[ whatpart ], p = part; *i; ++i, ++p){
   if ( p == part ){
      *p = toupper( *i );
   }else{
      *p = tolower( *i );
   }        
}
*p = 0;

free( pf );
return(1);
}
/*----------------------------------------------------------*/

int find_file(char *filename,char *gevonden,int *findex)
{
int     status;
struct  dsc$descriptor gevondend;
struct  dsc$descriptor filespec;
char    gevonden_file[NAM$C_MAXRSS + 1];

filespec.dsc$w_length = strlen(filename);
filespec.dsc$b_dtype  = DSC$K_DTYPE_T;
filespec.dsc$b_class  = DSC$K_CLASS_S; 
filespec.dsc$a_pointer = filename;

gevondend.dsc$w_length = NAM$C_MAXRSS;
gevondend.dsc$b_dtype  = DSC$K_DTYPE_T;
gevondend.dsc$b_class  = DSC$K_CLASS_S; 
gevondend.dsc$a_pointer = gevonden_file;

status=lib$find_file(&filespec,&gevondend,findex,0,0,0,0);
    
if ( (status & 1) == 1 ){
       strcpy(gevonden,strtok(gevonden_file," "));
}else{
       gevonden[0] = 0;
}

return(status);
}


/*--------------------------------------------*/

manPtr addman( manPtr *manroot,char *filename )
{
manPtr m,f;

m = calloc( 1, sizeof( man) );
if ( !m ) return( NULL );

m->filename = strdup( filename );

if ( *manroot == NULL ){
   *manroot = m;    
}else{
   for( f = *manroot; f->next ; f = f->next );
   f->next = m;
}
return(m);
}

/*--------------------------------------------*/
void freeman( manPtr *manroot )
{
manPtr m,n;

for( m = *manroot; m ; m = n ){
     free( m->filename );
     n = m->next;
     free ( m );
}
*manroot = NULL;
}

/*--------------------------------------------*/

int listofmans( char *filespec, manPtr *manroot )
{
manPtr  r;
int     status;
int     ffindex=0;
char    gevonden[NAM$C_MAXRSS + 1];

while(1){
    status = find_file( filespec, gevonden, &ffindex );

    if ( (status&1) ){
        r = addman( manroot, gevonden );
        if ( r == NULL ) return(2);
    }else{
        if ( !( status&1)) break;
    }
}

lib$find_file_end( &ffindex);
if ( status == RMS$_NMF) status = 1;


return( status );
}

/*--------------------------------------------*/

int convertman ( char *filespec, FILE *hlp , int base_level, int add_parentheses )
{
FILE    *man;
char    *in, *uit;
char    *m,*h;
size_t  len, thislen, maxlen= 50000;
int     bol,mode, return_status=1;
char subjectname[ NAM$C_MAXRSS + 1 ];

in  = calloc( 1, maxlen + 1 );
uit = calloc( 1, maxlen + 1 );

if ( in == NULL || uit == NULL ) return(2);

man = fopen( filespec, "r");
if ( man == NULL ) return(vaxc$errno);

for( len = 0; !feof( man ) && len < maxlen ; len += thislen ){
    thislen = fread( in + len, 1, maxlen - len, man );
}

fclose (man);

m = in;
h = uit;

*(m + len ) = 0;

for ( mode = 0, bol = 1 ; *m; ++m ){

    switch ( mode ){
        case 0:
          switch(*m){
            case '.':
                if ( bol ){
                    mode = 1;
                }else{
                    *h = *m;
                    ++h;
                }
                break;
            case '\\':
                if ( bol ){
                   *h = ' ';++h;
                   *h = ' ';++h;
                }
                mode = 2;
                break;
            default:
                if ( bol ){
                   *h = ' ';++h;
                   *h = ' ';++h;
                }
                *h = *m;
                ++h;
                break;
          }
          break;
        case 1: /* after . at bol */

          switch(*m){
            case '\\':
                while( *m != '\n' && *m != '\r' && *m )++m;
                mode = 0;
                break;
            case 'B':
                   ++m; 
                   *h = ' ';++h;
                   mode = 0;
                   break;   
            case 'I':
                    /* remove preceding eol */
                    if ( *(m+1) != 'P' ){
                        --h;
                        while ( (*h == '\n' || *h == '\r') && h > uit )--h;
                        ++h;
                    }

                    /* skip .Ix */
                    for(;*m != ' ' && *m != '\n' && *m != '\r'; ++m); 

                    /* copy line up to EOL */

                    for(;*m != '\n' && *m != '\r' && *m; ++m, ++h)*h = *m;

                    /* if line ends in ., this is an EOL */

                    if ( *(h-1) == '.'){
                         --h; 
                         --m;
                    }else{
                        /* if line does not end in ., skip EOL in source */

                        if ( *(m+1) == '\n' || *(m+1) == '\r')++m;
                    }
                    mode = 0;
                    break;
            case 'S':
                 if ( *(m+1) == 'H' ){
                    *h = '\n';++h;
                    if ( strncmp( m+3 ,"NAME",4) == 0 || 
                         strncmp( m+3 ,"SYNOPSIS",8) == 0 ||
                         strncmp( m+3 ,"DESCRIPTION",11) == 0 ){
                        while( *m != '\n' && *m != '\r')++m;
                        mode = 0;
                    }else{
                        ++m;

                        /* write help level, and flag it */

                        *h = '0' + base_level + 1;++h;
                        return_status |= 2;

                        *h = ' ';++h; 

                        /* skip H (or whatever after S) and blank */
                        ++m;++m;

                        for(;*m != '\n' && *m != '\r' && *m; ++m, ++h){

                           /* write help label in lowercase, skip quotes */
                           /* fill blanks with underscores */

                           if ( *m != '\"' ){
                                *h = tolower( *m );
                                if (*h == ' ') *h = '_';    
                           }else{
                                --h;
                           }    
                        } 

                        /* Add a linefeed or two */

                        *h = *m;++h;
                        *h = *m;++h;

                        mode = 0;
                    }   
                 }
                 break;
            case 'T':
                 if ( *(m+1) == 'H' ){
                    *h = '0' + base_level; ++h;
                    return_status |= 2;
                    *h = ' ';++h;
                    for ( m = m + 3; *m != ' ' && *m ; ++m, ++h ){
                          *h = *m;
                    }
					if ( add_parentheses ){
						 *h = '(';++h;
						 *h = ')';++h;
					}
                    while( *m != '\n' && *m != '\r' && *m )++m;
                    mode = 0;
                 }
                 break;
            default:
                ++m;
                mode = 0;
                break;
           }
           break;
        case 2: /* after \ skip two characters or print the backslash */            
          switch(*m){
            case '\\':
                *h = *m;
                ++h;
                mode = 0;
                break;
            default:
                ++m;
                mode = 0;
                break;
           }
           break;   
    } /*end switch mode */

    bol = 0;
    if ( *m == '\n' || *m == '\r') bol = 1;

}/* end for mode */

*h = 0;


if ( (return_status&2) ){
    fprintf( hlp, "%s\n\n", uit);
}else{
    fnamepart( filespec, subjectname,3);
    if ( *subjectname ){
        fprintf( hlp, "%d %s\n\n%s\n\n", base_level, subjectname, uit);
    }else{
        /* No filename (as is the case with a logical), use first word as subject name */
        char *n,*s;

        for(n = in; isspace( *n );++n);
        for(s = subjectname; !(isspace( *n )); ++n,++s)*s = *n;
        *s = 0;

        fprintf( hlp, "%d %s\n\n%s\n\n", base_level, subjectname, uit);
    }
}

/*
 printf( "read %d from %s, written %d to helpfile, return_status = %d\n",
    len, filespec, strlen(uit), return_status );
*/

free( m ); 
free( h ); 

return ( 1);
}

/*--------------------------------------------*/

int convertmans( char *filespec, char *hlpfilename, int base_level, int append, int add_parentheses )
{
int status=1;
manPtr  manroot=NULL, m;
FILE    *hlp;

if ( append ){
    hlp = fopen( hlpfilename,"a+");
}else{
    hlp = fopen( hlpfilename,"w");
}

if ( hlp == NULL ) return( vaxc$errno );

status = listofmans( filespec, &manroot );
if ( !(status&1) ) return( status );

for ( m = manroot ; m ; m = m->next ){
    status = convertman( m->filename, hlp , base_level, add_parentheses );
    if ( !(status&1) ){
        fprintf(stderr,"Convertman of %s went wrong\n", m->filename);
        break;
    }
}
freeman( &manroot );
return( status );
}

/*--------------------------------------------*/
void print_help()
{
   fprintf( stderr, "Usage: [-a] [-b x] convertman <manfilespec> <helptextfile>\n" );
   fprintf( stderr, "       -a append <manfilespec> to <helptextfile>\n" );
   fprintf( stderr, "       -b <baselevel> if no headers found create one with level <baselevel>\n" );
   fprintf( stderr, "          and the filename as title.\n" );
   fprintf( stderr, "       -p add parentheses() to baselevel help items.\n" );

}
/*--------------------------------------------*/

main ( int argc, char **argv )
{
int     status;
int     i,j;
int     append, base_level, basechange, add_parentheses;
char    *manfile=NULL;
char    *helpfile=NULL;

if ( argc < 3 ){
   print_help();
   return( 1 ) ;
}

append     = 0;
base_level = 1;
basechange = 0;
add_parentheses = 0;

for ( i = 1; i < argc; ++i){
    if ( argv[i][0] == '-' ){
        for( j = 1; argv[i][j] ; ++j ){
            switch( argv[i][j] ){
                case 'a':
                    append = 1;
                    break;
                case 'b':   
                    if ( (i+1) < argc ){
                        base_level = atoi( argv[ i + 1 ] );
                        basechange = 1;
                    }
                    break;
                case 'p':
                    add_parentheses = 1;
                    break;
            }
        }
        if ( basechange){
            basechange = 0;
            i = i + 1;
        }
    }else{
        if ( manfile == NULL ){
            manfile = strdup( argv[i]);
        } else if ( helpfile == NULL ){
            helpfile = strdup( argv[i]);
        } else {
            fprintf( stderr, "Unrecognized parameter : %s\n", argv[i]);
        }
    }
}


/* fprintf( stderr,"manfile: %s, helpfile: %s, append: %d, base_level : %d\n",
        manfile, helpfile, append, base_level);
*/

status = convertmans( manfile, helpfile, base_level, append, add_parentheses );

free( manfile );
free( helpfile );

return( status );
}

 
