#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <cyg/fileio/fileio.h>
#include <cyg/infra/diag.h>
#include <cyg/kernel/kapi.h>
#include <cyg/io/aud/data_transfer.h>
#include <cyg/io/aud/adecctrl.h>
#include <cyg/io/aud/UB.h>
#include <pkgconf/io_aud.h>
#include <pkgconf/devs_mad_mp3.h>
#include <sys/stat.h>


// To run SD test
#ifdef USE_SD
#include <cyg/io/disk.h>
#include <cyg/io/sd_sis910.h>
#include <cyg/io/mmc_protocol.h>
#include <cyg/io/sd_protocol.h>
#endif
#define STACK_SIZE		65535

#ifdef AUD_DEBUG 
#define EPRINTF(a)	printf a
#else
#define EPRINTF(a)
#endif


#ifdef OUTPUT_WAV
extern char outpath[];
#endif

//==========================================================================
char threadI_stack[STACK_SIZE];
char threadII_stack[STACK_SIZE];
static AUD_tCtrlBlk Acb = {0};

extern struct AUD_tModule MAD_ModuleMP3;
extern cyg_handle_t  ABV_thread;
extern cyg_thread ABV_handle;
extern cyg_handle_t  decoder_thread;
extern cyg_thread	decoder_handle;
extern uint file_size;

#ifdef OUTPUT_WAV
extern char outpath[];
#endif

#ifdef OUTPUT_DATA_BY_NET
extern int dir_deep;
extern char net_outdir[][64];
extern char net_outfile[];
#endif

//#define BAT_PROCESS 1
#define SEC_DIR		"FOLK"

//==================================================

//void ABV_loop(cyg_addrword_t arg)
void ABV_loop( )
{	
	FILE * mp3 = NULL;
	int err = 0, fsize = 0, ret = 0;

	char *tmp = NULL;
	struct stat *buf = NULL;
	
	
    printf("into ABV_loop!\n");
#ifdef USE_SD
    err = mount( "/dev/sdblk0/", "/", "fatfs" );
#else    
    err = mount("/dev/hda/1", "/", "fatfs");
#endif

    if(err < 0) 
		EPRINTF(("Mount File System Error\n"));
	else
		EPRINTF(("Mount File System Succeed\n"));

#ifdef OUTPUT_WAV
	strcpy(outpath ,"/FOLK01C.WAV");
#endif

	if( NULL == (mp3 = fopen( "/LOVE.mp3", "rb+" )) )
	{
		EPRINTF(("Failed to open file!!!\n"));
    }
    else
    {
        EPRINTF(("open mp3 src file success!!!\n"));
    }
    buf = (struct stat *)malloc(sizeof(struct stat));
    err = stat("/LOVE.mp3", buf);
    if(err < 0)
    {
        EPRINTF(("stat file error!!!\n"));
    }
    else
    {
        EPRINTF(("stat file success!!!\n"));
    }
    
	fsize = buf->st_size;
	EPRINTF(("file size is:%d!!!\n", fsize));
	EPRINTF(("File total size = %d\n", fsize));		
	EPRINTF(("Write ABV Buffer Begin\n"));
		
	ABV_init( &Acb.Abv, fsize, fsize );
	
	
    fread(Acb.Abv.start,1,fsize,mp3);
    //set up abv data
    Acb.Abv.wr_ok = 1;
    Acb.Abv.wrptr = Acb.Abv.start + fsize;
    Acb.Abv.end = Acb.Abv.start + fsize -1;
	diag_printf("start:%x\n",Acb.Abv.start);		
	//ABV_write( mp3 );
	EPRINTF(("Write ABV Buffer Over\n"));

	if(mp3 != NULL)
	{
		EPRINTF(("Close MP3 file\n"));
		fclose(mp3);    
	}
	
	return;
}

//==========================================================================


void MP3_Decode( )
{ 
    int ret = 0;
    //cyg_thread_delay(15000);
    
    EPRINTF(("Come into MP3_Decode thread\n"));
    
    ret = MAD_ModuleMP3.Init(&Acb);
    if( 1 == ret )
    { 
    	printf("^^MP3 Decoder Init OK!\n");
    }
    ret = MAD_ModuleMP3.Decode(&Acb);	
    if( 1 == ret ) 
    	printf("MP3 Decoder Done!\n");
    
    MAD_ModuleMP3.Cleanup(&Acb);
}

void mp3_thread_decode()
{
    ABV_loop();
    MP3_Decode(); 
}
void cyg_user_start(void)
{    		
	/* create thread  */
	printf("into mp3 decoder user start!\n");
 	cyg_thread_create(
			10,
			mp3_thread_decode,
			(cyg_addrword_t)75,
			"Thread mp3",
			(void *)threadI_stack,
			STACK_SIZE,
		    &decoder_thread,
		    &decoder_handle );
        printf("cyg_thread_create MP3_Decode success!\n");
        cyg_thread_resume(decoder_thread);	//Start it!
        //cyg_scheduler_start();
        //mp3_thread_decode();		    
}
