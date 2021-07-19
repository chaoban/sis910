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
#include <pkgconf/devs_aud_mp3.h>
#include "debug.h"
#include <sys/stat.h>


// To run SD test
#include <cyg/io/disk.h>
#include <cyg/io/sd_sis910.h>
#include <cyg/io/mmc_protocol.h>
#include <cyg/io/sd_protocol.h>

#define STACK_SIZE		65535


//==========================================================================
char threadI_stack[STACK_SIZE];
char threadII_stack[STACK_SIZE];
static AUD_tCtrlBlk Acb = {0};

extern struct AUD_tModule AUD_ModuleMP3;
extern int AUD_MP3_Init(AUD_tCtrlBlk *pAcb);
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

void ABV_loop(cyg_addrword_t arg)
{	
	FILE * mp3 = NULL;
	int err = 0, fsize = 0, ret = 0;

	char *tmp = NULL;
	struct stat *buf = NULL;
	
#if BAT_PROCESS
	DIR *dirp;
	char outdir[24] = "/dec_res/";
	char indir[48] = "/PATTERN/";
	struct dirent *dir, *entry, *child;
#endif
#ifdef OUTPUT_DATA_BY_NET
	char cmd[8];
#endif	
	
    printf("into ABV_loop!\n");
    //err = mount("/dev/hda/1", "/", "fatfs");
    err = mount( "/dev/sdblk0/", "/", "fatfs" );

    if(err < 0) 
		EPRINTF(("Mount File System Error\n"));
	else
		EPRINTF(("Mount File System Succeed\n"));

#if BAT_PROCESS	

#ifdef OUTPUT_WAV_BY_NET
	err = NET_Data_init();
	if(-1 == err)
		return;
#endif

	dirp = opendir( "/" );	
	for(;;){
		dir = readdir( dirp );		
		if(err = strstr(dir->d_name, "PATTERN")){
	        	//diag_printf("Come in %14s\n",dir->d_name);
			chdir("/PATTERN");
			dirp = opendir("/PATTERN");
			for(;;){
				dir = readdir( dirp );	
				if(err = strstr(dir->d_name, SEC_DIR)){	
#ifdef OUTPUT_WAV_BY_NET
					strcpy(net_outdir[dir_deep], SEC_DIR);
					dir_deep = 1;
					strcpy(cmd, "-d 1");
					NET_Data_transfer(cmd, strlen(cmd)+1);
					NET_Data_transfer(net_outdir[dir_deep-1], strlen(net_outdir[dir_deep-1])+1);					
					cyg_thread_delay(250);		// delay for mkdir 
#endif
					strcat(indir, SEC_DIR);
					chdir(indir);
					diag_printf("Come in %14s\n",indir);
					dirp = opendir(indir);
					break;
				}
			}
			break;
		}
	}
	
	for(;;)
	{
		child = readdir( dirp );
		if( child == NULL ){
			diag_printf("no folder\n");
			break;
		}

		if(child->d_name[0] >= 'A')
			diag_printf("Come in %14s\n", child->d_name);
		else
			continue;
		switch( child->d_name[0] ){
			case 'A':	
				strcat(indir, "/A");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/A/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET				
				strcpy(net_outdir[1], "A");		
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif
				break;
			case 'B':	
				strcat(indir, "/B");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/B/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "B");		
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif
				break;
			case 'C':	
				strcat(indir, "/C");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/C/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "C");		
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif				
				break;
			case 'D':	
				strcat(indir, "/D");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/D/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "D");	
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif				
				break;
			case 'E':	
				strcat(indir, "/E");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/E/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "E");		
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif				
				break;
			case 'F':	
				strcat(indir, "/F");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/F/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "F");		
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif				
				break;
			case 'G':	
				strcat(indir, "/G");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/G/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "G");	
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif				
				break;
			case 'H':	
				strcat(indir, "/H");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/H/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "H");	
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif				
				break;
			case 'I':	
				strcat(indir, "/I");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/I/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "I");	
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif				
				break;
			case 'J':	
				strcat(indir, "/J");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/J/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "J");		
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif				
				break;
			case 'K':	
				strcat(indir, "/K");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/K/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "K");	
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif				
				break;
			case 'L':	
				strcat(indir, "/L");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/L/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "L");	
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif				
				break;
			case 'M':	
				strcat(indir, "/M");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/M/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "M");	
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif				
				break;
			case 'N':	
				strcat(indir, "/N");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/N/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "N");	
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif				
				break;
			case 'O':	
				strcat(indir, "/O");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/O/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "O");	
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif				
				break;
			case 'P':	
				strcat(indir, "/P");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/P/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "P");		
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif				
				break;
			case 'Q':	
				strcat(indir, "/Q");
				strcat(outdir, SEC_DIR);
				strcat(outdir, "/Q/");
				chdir(indir);
				diag_printf("Come in %14s\n", indir);
				child = opendir(indir);
				strcpy(indir,"/PATTERN/");
				strcat(indir, SEC_DIR);
#ifdef OUTPUT_WAV_BY_NET
				strcpy(net_outdir[1], "Q");	
				dir_deep = 2;
				strcpy(cmd, "-d 2");
			NET_Data_transfer(cmd, strlen(cmd)+1);
			NET_Data_transfer(net_outdir[1], strlen(net_outdir[1])+1);
#endif				
				break;
			default:
				break;
		}
			
			for(;;){
			entry = readdir( child );
			if( entry == NULL ){
				printf("no entry\n");
	            break;
			}
			
			if(err = strstr(entry->d_name, ".MP3"))
	        		diag_printf("Open MP3: %14s\n",entry->d_name);
			else 
				continue;
			
			if( NULL == (mp3 = fopen( entry->d_name, "rb+" )) ){
				EPRINTF(("Failed to open file %s\n", entry->d_name));
				break;
	    		}

#ifdef OUTPUT_DATA_BY_NET
			strcpy(net_outfile, entry->d_name);
#endif

#ifdef OUTPUT_WAV
			strcpy(outpath, outdir);	
			tmp = strtok(entry->d_name, ".");
			tmp = strcat(tmp, ".wav");
			strcat(outpath, tmp);
			printf("Output wav: %14s\n", outpath);
#endif						

			/* create a thread for ABV writing */	
			while(feof(mp3) == 0){
				fgetc(mp3);
				fsize ++;
			}
			fseek(mp3, 0, SEEK_SET);
			
			EPRINTF(("File total size = %d\n", fsize));		
			EPRINTF(("Write ABV Buffer Begin\n"));

			
			ABV_init( &Acb.Abv, 1048576 * 16, fsize );	
			
			ABV_write( mp3 );	
			
			EPRINTF(("Write ABV Buffer Over\n"));

			EPRINTF(("Come into MP3_Decode thread\n"));

			ret = AUD_ModuleMP3.Init(&Acb);
			if( 1 == ret ) printf("MP3 Decoder Init OK!\n");
			else break;
			
			ret = AUD_ModuleMP3.Decode(&Acb);	
			if( 1 == ret ) printf("MP3 Decoder Done!\n");
			else break;

			AUD_ModuleMP3.Cleanup(&Acb);		
			printf("Cleanup ABV Buffer\n");
			ABV_exit();
			
			if(mp3 != NULL){
				EPRINTF(("Close MP3 file\n"));
			    	fclose(mp3);    
				fsize = 0;
			}
		}		

		strcpy(outdir, "/dec_res/");
	}    			
#else //BAT_PROCESS  no bat process
	dirp = opendir( "/" );	
	for(;;){
		dir = readdir( dirp );		
		if(err = strstr(dir->d_name, "PATTERN")){
	        	diag_printf("Come in %14s\n",dir->d_name);
			chdir("/PATTERN");
			dirp = opendir("/PATTERN");
			for(;;){
				dir = readdir( dirp );	
				if(err = strstr(dir->d_name, "FOLK")){
					diag_printf("Come in %14s\n",dir->d_name);
					chdir("/PATTERN/FOLK");
					dirp = opendir("/PATTERN/FOLK");
					break;
				}
			}
			break;
		}
	}

	for(;;){
		entry = readdir( dirp );
		if( entry == NULL ){
			printf("no entry\n");
            break;
		}
		//printf("%14s\n", entry->d_name);
		if(err = strstr(entry->d_name, "FOLK01C.MP3")){
        		diag_printf("Open MP3: %14s\n",entry->d_name);
			break;
		}
		else 
			continue;
	}

#ifdef OUTPUT_WAV
	strcpy(outpath ,"/FOLK01C.WAV");
#endif

	if( NULL == (mp3 = fopen( entry->d_name, "rb+" )) ){
		EPRINTF(("Failed to open file %s\n", entry->d_name));
    	}

	/* create a thread for ABV writing */	
		while(feof(mp3) == 0){
			fgetc(mp3);
			fsize ++;
		}
		fseek(mp3, 0, SEEK_SET);
		
		EPRINTF(("File total size = %d\n", fsize));		
		EPRINTF(("Write ABV Buffer Begin\n"));
		
	ABV_init( &Acb.Abv, 1048576 * 4, fsize );
	ABV_write( mp3 );
	EPRINTF(("Write ABV Buffer Over\n"));

	if(mp3 != NULL)
	{
		EPRINTF(("Close MP3 file\n"));
		fclose(mp3);    
	}

#endif //BAT_PROCESS
	
	return;
}

//==========================================================================

#if BAT_PROCESS
#else
void MP3_Decode(cyg_addrword_t arg)
{ 
	int ret;

		Acb.Abv.wr_ok = 0;
		cyg_thread_delay(1500);
		
		EPRINTF(("Come into MP3_Decode thread\n"));

		ret = AUD_ModuleMP3.Init(&Acb);
		if( 1 == ret ) 
			printf("MP3 Decoder Init OK!\n");
		
		ret = AUD_ModuleMP3.Decode(&Acb);	
		if( 1 == ret ) 
			printf("MP3 Decoder Done!\n");

		AUD_ModuleMP3.Cleanup(&Acb);
}
#endif

void cyg_user_start(void)
{    		
	/* create thread  */
	cyg_thread_create( 10, ABV_loop, 0, "ABV_loop", 
		(void *)threadI_stack, STACK_SIZE, &ABV_thread, &ABV_handle );	
#if BAT_PROCESS
#else
	cyg_thread_create( 10, MP3_Decode, 0, "MP3_Decode", 
	(void *)threadII_stack, STACK_SIZE, &decoder_thread, &decoder_handle);
#endif

	/* start thread */
	cyg_thread_resume( ABV_thread );
#if BAT_PROCESS
#else
	cyg_thread_resume(decoder_thread);
#endif
}
