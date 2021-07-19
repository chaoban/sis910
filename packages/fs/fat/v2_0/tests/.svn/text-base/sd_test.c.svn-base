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

int main()
{
     char buf[IOSIZE];
     int in, out;
     int nbytes,done,i,j,count;
     int err;
     

    
    err = mount( "/dev/sdblk0/", "/", "fatfs" );

    if (err)
    {
        diag_printf(" Mount fails!; err = %d\n", err);
        diag_printf("Prepare to mount SD disk on partition 1.\n");
        err = mount( "/dev/sdblk0/1", "/", "fatfs" );
        if (err)
        {
            diag_printf(" Mount fails!\n");
        }
    }
    else
    {
        diag_printf(" Mount success!\n");
    }
   
     nbytes = sizeof(buf);

     in = open("/bigfile/2.t", O_RDONLY);
     if( in == -1 )
       {
            diag_printf("open in error %d\n",in);
            return 0;
        }

     out = open("995.txt", O_WRONLY|O_CREAT);
     if( out == -1 )
     {
       diag_printf("open out error %d\n",out);
       return 0;
     }  

    for(;;)
    {
        done = read( in, buf, nbytes );
       
        if( done <= 0 ) break;

        err = write(out, buf, done);
        
        if( err < 0 )
        {
            diag_printf("write error %d\n",err);
            break;
        }

   }
     close(in);
     close(out);

     diag_printf("finish\n");
     return 0;
}
