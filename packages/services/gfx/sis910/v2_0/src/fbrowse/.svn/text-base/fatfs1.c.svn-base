
#ifdef ECOS

#include <pkgconf/hal.h>
#include <pkgconf/io_fileio.h>
#include <pkgconf/fs_fat.h>

#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros


#include <cyg/fileio/fileio.h>

#include <cyg/infra/testcase.h>
#include <cyg/infra/diag.h>            // HAL polled output
#include <cyg/fs/fatfs.h>
#endif

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>


#include "tinywidget/tnWidgets.h"



//==========================================================================
#define diag_printf printf
#define CYG_TEST_FAIL printf

#define SHOW_RESULT( _fn, _res ) \
diag_printf("<FAIL>: " #_fn "() returned %d %s\n", _res, _res<0?strerror(errno):"");

//==========================================================================

#define IOSIZE  100

#define LONGNAME1       "long_file_name_that_should_take_up_more_than_one_directory_entry_1"
#define LONGNAME2       "long_file_name_that_should_take_up_more_than_one_directory_entry_2"


extern TN_WIDGET *bar_state;
//==========================================================================

#ifndef CYGPKG_LIBC_STRING

char *strcat( char *s1, const char *s2 )
{
    char *s = s1;
    while( *s1 ) s1++;
    while( (*s1++ = *s2++) != 0);
    return s;
}

#endif

//==========================================================================

//==========================================================================

static void createfile( char *name, size_t size )
{
    char buf[IOSIZE];
    int fd;
    ssize_t wrote;
    int i;
    int err;

    diag_printf("<INFO>: create file %s size %zd \n",name,size);

    err = access( name, F_OK );
    if( err < 0 && errno != EACCES ) SHOW_RESULT( access, err );
    
    for( i = 0; i < IOSIZE; i++ ) buf[i] = i%256;
 
    fd = open( name, O_WRONLY|O_CREAT );
    if( fd < 0 ) SHOW_RESULT( open, fd );

    while( size > 0 )
    {
        ssize_t len = size;
        if ( len > IOSIZE ) len = IOSIZE;
        
        wrote = write( fd, buf, len );
        if( wrote != len ) SHOW_RESULT( write, (int)wrote );        

        size -= wrote;
    }

    err = close( fd );
    if( err < 0 ) SHOW_RESULT( close, err );
}

//==========================================================================


//==========================================================================


//==========================================================================

void copyfile( char *name2, char *name1 )
{

    int err;
    char buf[IOSIZE];
    int fd1, fd2;
    ssize_t done, wrote;
		GrMapWindow(bar_state->wid);
		off_t file_length;
		struct stat s;
		float bar_pos = 0;
			
    printf("<INFO>: copy file %s -> %s\n",name2,name1);
/*
    err = access( name1, F_OK );
    if( err < 0 && errno != EACCES ) SHOW_RESULT( access, err );
*/
    err = access( name2, F_OK );
    if( err != 0 ) SHOW_RESULT( access, err );
    
    stat(name2,&s);
    	file_length = s.st_size;
  			float step1 = (IOSIZE*100.0/file_length);
    
    fd1 = open( name1, O_WRONLY|O_CREAT );
    if( fd1 < 0 ) 
    	{	printf("can not create file %s !!\n",name1);
    			return;	
    		}
    		
    fd2 = open( name2, O_RDONLY );
    if( fd2 < 0 ) 
    		{	printf("can not open %s!!\n",name2);
    				return;
    		}
    
    
    
    
    for(;;)
    {
        done = read( fd2, buf, IOSIZE );
        if( done < 0 ) SHOW_RESULT( read, (int)done );

        if( done == 0 ) break;

        wrote = write( fd1, buf, done );
        if( wrote != done ) SHOW_RESULT( write, (int) wrote );

        if( wrote != done ) break;
        		bar_pos+=step1;
        		tnProgressBarUpdate(bar_state,(int)bar_pos);
    }

    err = close( fd1 );
    if( err < 0 ) 
    	printf("close file target file error!!\n");
    
    err = close( fd2 );
    if( err < 0 )
    	printf("close source file error!!\n");
    
}



int delfile(const char *file_path)
{	int n =unlink(file_path);
   if(n<0)
   {printf("delete file :%s failed!!\n",file_path);
	return -1;	
	}
	return 1;
}
/*
-------------------------------------------------------------

    diag_printf("<INFO>: unlink fee\n");    
    err = unlink( "/fee" );
    if( err < 0 ) SHOW_RESULT( unlink, err );

    diag_printf("<INFO>: unlink fum\n");        
   
*/
