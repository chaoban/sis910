#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <cyg/io/sd_sis910.h>
#include <cyg/io/mmc_protocol.h>
#include <cyg/io/sd_protocol.h>
#include <cyg/infra/diag.h>
#include <math.h>
#include <errno.h>
#include <cyg/fileio/fileio.h>
#include <string.h>
#include <stdio.h>
#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>

#define IOSIZE 512

int copy1(char *src, char *dst)
{
     char buf[IOSIZE];
     int in, out;
     int nbytes,done;
     int err;

    diag_printf("copy1 use read write!\n");
     nbytes = sizeof(buf);
    
     in = open(src, O_RDONLY);
     if( in == -1 )
       {
            diag_printf("open in error %d\n",in);
            return 0;
        }
    
     out = open(dst, O_WRONLY|O_CREAT);
     if( out == -1 )
     {
       diag_printf("open out error %d\n",out);
       return 0;
     }  
    
    for(;;)
    {
        done = read( in, buf, nbytes );
       
        if( done <= 0 )
        {
            diag_printf("read error %d\n",done);
            break;
        }

        err = write(out, buf, done);
        if( err < 0 )
        {
            diag_printf("write error %d\n",err);
            break;
        }
    
    }
     close(in);
     close(out);    
    return 0;
}

void copy2(char *src, char *dst)
{
    FILE *pOpenFile = NULL, *pWriteFile = NULL;
    char buffer[4096];
//    char c;

    diag_printf("copy2 use fgets fputs!\n");

	pOpenFile = fopen(src, "r" );
    if (pOpenFile == NULL)
    {
	    diag_printf("Failed to open file %s %d\n", "io_read.txt", errno);
	}
    else
    {
        diag_printf("success to open file %s\n", "io_read.txt");
    }

	pWriteFile = fopen(dst, "w" );
    if (pWriteFile == NULL)
    {
	    diag_printf("Failed to open file %s %d\n", "iotest.txt", errno);
	}
    else
    {
        diag_printf("success to open file %s\n", "iotest.txt");
    }

//    sprintf(str, "%08X", 123456789);
    while(!feof(pOpenFile))
    {
        fgets(buffer, 4096, pOpenFile);
        fputs(buffer, pWriteFile);
		//diag_printf("Write 4096 byte\n");
    }

    fclose(pWriteFile);
    diag_printf("close to open file %s\n", "iotest.txt");
    
    fclose(pOpenFile);
    diag_printf("close to open file %s\n", "io_read.txt");    
    
    
}

void copy3(char *src, char *dst)
{
    FILE *pOpenFile = NULL, *pWriteFile = NULL;
//    char buffer[4096];
    char c;

    diag_printf("copy3 use fgetc fputc!\n");

	pOpenFile = fopen(src, "r" );
    if (pOpenFile == NULL)
    {
	    diag_printf("Failed to open file %s %d\n", "io_read.txt", errno);
	}
    else
    {
        diag_printf("success to open file %s\n", "io_read.txt");
    }

	pWriteFile = fopen(dst, "w" );
    if (pWriteFile == NULL)
    {
	    diag_printf("Failed to open file %s %d\n", "iotest.txt", errno);
	}
    else
    {
        diag_printf("success to open file %s\n", "iotest.txt");
    }

//    sprintf(str, "%08X", 123456789);
    while((c = fgetc(pOpenFile)) != EOF)
    {
//        fgets(buffer, 4096, pOpenFile);
//        fputs(buffer, pWriteFile);

 //       c = fgetc(pOpenFile);
        fputc(c, pWriteFile);
		//diag_printf("Write 4096 byte\n");
    }

    fclose(pWriteFile);
    diag_printf("close to open file %s\n", "iotest.txt");
    
    fclose(pOpenFile);
    diag_printf("close to open file %s\n", "io_read.txt");    
    
    
}
void copy4(char *src, char *dst)
{
    FILE *pOpenFile = NULL, *pWriteFile = NULL;
    char buffer1[512];
    int count = 0;
//    char buffer2[512];
//    char c;

    diag_printf("copy4 use fread fwrite!\n");

	pOpenFile = fopen(src, "r" );
//    if (pOpenFile == NULL)
//    {
//	    diag_printf("Failed to open file %s %d\n", "io_read.txt", errno);
//	}
//    else
//    {
//        diag_printf("success to open file %s\n", "io_read.txt");
//    }

	pWriteFile = fopen(dst, "w" );
//    if (pWriteFile == NULL)
//    {
//	    diag_printf("Failed to open file %s %d\n", "iotest.txt", errno);
//	}
//    else
//    {
//        diag_printf("success to open file %s\n", "iotest.txt");
//    }

//    sprintf(str, "%08X", 123456789);
    while(!feof(pOpenFile))
    {
        
        count = fread (buffer1,1,sizeof(buffer1),pOpenFile);
//        if(count != sizeof(buffer1))
  		    fwrite(buffer1,1,count,pWriteFile);
//  		else
//  		    fwrite(buffer1,1,sizeof(buffer1),pWriteFile);
    }

    fclose(pWriteFile);
    diag_printf("close to open file %s\n", "iotest.txt");
    
    fclose(pOpenFile);
    diag_printf("close to open file %s\n", "io_read.txt");    
    
    
}
int main()
{
    int err;
    cyg_tick_count_t ticks;
    
    err = mount( "/dev/sdblk0/", "/", "fatfs" );

    if (err)
    {
        diag_printf(" Mount fails!; err = %d\n", err);
        diag_printf("Prepare to mount SD disk on partition 1.\n");
        mount( "/dev/sdblk0/1", "/", "fatfs" );
        if (err)
        {
            diag_printf(" Mount fails!\n");
        }
    }
    else
    {
        diag_printf(" Mount success!\n");
    }
//ticks = cyg_current_time();
//printf("Starr Time is %llu\n", ticks);
//    copy1("io_read.txt", "test2.txt");  // use read write
//    copy2("io_read.txt", "iotest.txt"); //use fgets fputs
//    copy3("io_read.txt", "iotest.txt"); //use fgetc fputc
    copy4("io_read.txt", "test06.txt");
//ticks = cyg_current_time();
//printf("End Time is %llu\n", ticks);    
    
    diag_printf("finish\n");
    return 0;
}
