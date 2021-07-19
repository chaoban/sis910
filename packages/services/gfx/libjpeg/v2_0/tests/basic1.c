#if 0   
1.degug 
    diag_printf("%s:%d (%s)\n", __FUNCTION__, __LINE__, __FILE__);
2.memory
    diag_mallocinfo();
3.step by step
    Getch();
#endif
#include <pkgconf/kernel.h>
#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL
#include <cyg/hal/hal_intr.h>
#include <cyg/kernel/kapi.h>	// Kernel API.
#include <cyg/infra/diag.h>		// For diagnostic printing.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cyg/fileio/fileio.h>
// To test FAT
#include <pkgconf/io_fileio.h>
#include <pkgconf/fs_fat.h>
#include <cyg/fs/fatfs.h>

#include <jpeg/jpeglib.h>
#include <cyg/io/display/crt/Crt_io.h>
#include <cyg/io/display/g2d/G2D_io.h>

//Ram disk
#include <pkgconf/hal.h>
#include <pkgconf/isoinfra.h>
#include <pkgconf/system.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/testcase.h>

// SD test
#include <cyg/io/disk.h>
#include <cyg/io/sd_sis910.h>
#include <cyg/io/mmc_protocol.h>
#include <cyg/io/sd_protocol.h>

//#define __performance 1
//#define __degug_mode 1
//#define __demo_show 1
//#define __write_bmp 1
#define __h_jpeg 1

//#define dir_list 1

//*********************************************************************
// verify
//#define verify_prescaling      1
//#define verify_shift_problem   1
//#define verify_jpeg_basic_test 1
#define show_dir_list          1
//*********************************************************************

#ifdef verify_jpeg_basic_test
#define slice_view 1
#endif

#ifdef __degug_mode
#define APprintf(x)  diag_printf(x)
#else
#define APprintf(x)
#endif

/* Thread JPEG */
#define NTHREADS 		1
#define STACKSIZE 		4096

#define THREAD_STACK_SIZE	((8192 / sizeof(int)))
#define THREAD_PRIORITY		12

int thread_a_stack[ THREAD_STACK_SIZE ];
cyg_thread thread_a_obj;
cyg_handle_t thread_a_hdl;

cyg_sem_t sem_signal_thread;

/* Function */
extern char Getch(void);

static void thread_jpeg_decode( cyg_addrword_t data );
void Bilinear(cyg_uint8 *src, int s_w, int s_h, int s_pitch, int d_pitch, cyg_uint8 *dst, float h_sca, float v_sca);

/* Performance Test */
extern void tick2time(cyg_tick_count_t t);
extern void wait_for_tick(void);
extern void show_test(void);

#ifdef __performance
cyg_tick_count_t t1 = 0, t2 = 0; 
static cyg_tick_count_t t_hwdecode = 0, t_swdecode = 0, t_ldfile = 0, t_swsca = 0;
cyg_tick_count_t tc_poll, tc_vertical, tc_horizon; 
int tc_subhorizon[256];
int subcount, tc_totalsubhor;
cyg_bool martixfull;

//extern cyg_tick_count_t tc_poll, tc_vertical, tc_horizon; 
//extern int tc_subhorizon[256];
//extern int subcount, tc_totalsubhor;
//extern cyg_bool martixfull;

void tick2time(cyg_tick_count_t t);
void wait_for_tick(void);
void show_test(void);

//#define TICKS_PER_SECOND	37.4574
#define TICKS_PER_SECOND	100

void show_test(void)
{
   	diag_printf("=Open File:");
	tick2time(t_ldfile);
	diag_printf("SW Prepare:");
	tick2time(t_swdecode);
	diag_printf("JPEG HW Decode:");
	tick2time(t_hwdecode);
    diag_printf("\n");
}

void wait_for_tick(void)
{
    cyg_tick_count_t tv0, tv1;
    tv0 = cyg_current_time();
    while (true) {
        tv1 = cyg_current_time();
        if (tv1 != tv0) break;
    }
}

void tick2time(cyg_tick_count_t t)
{
    cyg_uint32 totalseconds = t/TICKS_PER_SECOND;
    cyg_uint32 tempseconds = totalseconds;
    cyg_uint32 day = 0;
    cyg_uint32 hour = 0;
    cyg_uint32 minute = 0;
    cyg_uint32 sec = 0;
    cyg_uint32 msec = 0;

	/* DAY */
    if(tempseconds >= 86400){
		day = (tempseconds / 86400);
		tempseconds -= (day * 86400);
    }
    /* HOUR */
    if(tempseconds >= 3600){
		hour = (tempseconds/3600);
		tempseconds -= (hour*3600);
    }
    /* MINUTE */
    if(tempseconds >= 60){
		minute = (tempseconds/60);
		tempseconds -= (minute*60);
    }
    /* SECOND */
    sec = tempseconds;
    
    /* 1/100 SECOND */
//	msec = (tempseconds*1000);
//	msec = (msec%1000)/10;
    msec = t%TICKS_PER_SECOND;
    
    if(day > 0){
        diag_printf("  %d day %d hour %d min %d.%02d sec. ", day, hour, minute, sec, msec);
    } 
    else if(hour > 0){
        diag_printf("  %d hour %d min %d.%02d sec. ", hour, minute, sec, msec);
    }
    else if(minute > 0){
        diag_printf("  %d min %d.%02d sec. ", minute, sec, msec);
    }
    else{
        diag_printf("  %d.%02d sec. ", sec, msec);
    }
}
#endif

#define pixel(ab,r,g,b) (((ab&0xff)<<24)|((r&0xff)<<16)|((g&0xff)<<8)|((b&0xff)<<0))

display_info show_buffer={0};

cyg_uint8 *a = NULL, *b = NULL;
lcd_info lcd_info_QQ = {0};

#define SHOW_RESULT( _fn, _res ) \
  diag_printf("<FAIL>: " #_fn "() returned %d %s\n", (int)_res, _res<0?strerror(errno):"");

typedef struct djpeg_dest_struct * djpeg_dest_ptr;

struct djpeg_dest_struct {
  /* start_output is called after jpeg_start_decompress finishes.
   * The color map will be ready at this time, if one is needed.
   */
  JMETHOD(void, start_output, (j_decompress_ptr cinfo,
			       djpeg_dest_ptr dinfo));
  /* Emit the specified number of pixel rows from the buffer. */
  JMETHOD(void, put_pixel_rows, (j_decompress_ptr cinfo,
				 djpeg_dest_ptr dinfo,
				 JDIMENSION rows_supplied));
  /* Finish up at the end of the image. */
  JMETHOD(void, finish_output, (j_decompress_ptr cinfo,
				djpeg_dest_ptr dinfo));

  /* Target file spec; filled in by djpeg.c after object is created. */
  FILE * output_file;

  /* Output pixel-row buffer.  Created by module init or start_output.
   * Width is cinfo->output_width * cinfo->output_components;
   * height is buffer_height.
   */
  JSAMPARRAY buffer;
  JDIMENSION buffer_height;
};

typedef struct my_dest_struct * my_dest_ptr;

struct my_dest_struct 
{
	char* imagebits;
	cyg_uint16 nwidth;
	cyg_uint16 nheight;
	unsigned int npitch;
};

typedef struct {  /* structure for reading images from buffer   */
	unsigned char *start;	/* The pointer to the beginning of the buffer */
	unsigned long offset;	/* The current offset within the buffer       */
	unsigned long size;	/* The total size of the buffer               */
} buffer_t;

typedef struct hw_option * hw_option_ptr;

struct hw_option{
    cyg_uint8 sample_rate;
    boolean filter_enable;
    cyg_uint8 saving_mode;
};

void int2str(int ,char *);
void g2d_mem2file(cyg_uint8 *, int, int, int, FILE *, cyg_uint8);
#ifdef dir_list
static void listdir_(char *name, int statp );
#endif
int gdecodejpeg(const char *, const char *, hw_option_ptr);
void show_name(const char *);
void inverse_array(cyg_uint8 *, int, int);
void write_frame1(int,int,cyg_uint8 *, cyg_uint8, cyg_uint8 *);
void g2d_4to3(cyg_uint8 *src, int w, int h, int pitch, cyg_uint8 *dst);
void lag_rc(int x);

//pitch=3*w (RGB)  + write bmp
void inverse_array(cyg_uint8 *src,int pitch,int h)
{
	int i = 0, j = 0;
	int end = h >> 1;
	cyg_uint8 temp = 0;
	cyg_uint32 temp1 = 0, temp2 = 0; 
	for (j = 0 ; j < end ; j ++)
	{
		for (i = 0 ; i < pitch ; i++)
		{
			temp1 = j * pitch + i;
			temp2 = (h - 1 - j) * pitch + i;
			temp = src[temp1];
			src[temp1] = src[temp2];
			src[temp2] = temp;
		}
	}
}

void int2str(int i,char *s)
{
    sprintf(s, "%d", i);
}

void g2d_4to3(cyg_uint8 *src, int w, int h, int pitch, cyg_uint8 *dst)
{
    int i = 0, j = 0, k = 0;
    cyg_uint32 src_temp = 0, dst_temp = 0, ctemp = 0, dtemp = 0;
    cyg_uint32 dst_pitch = w * 3; 
    for(j = 0; j < h; j ++)
    {
        ctemp = j * pitch;
        dtemp = j * dst_pitch;
        for(i = 0, k = 0; i < pitch; i += 4, k += 3)
        {
            dst_temp = dtemp + k;
            src_temp = ctemp + i;
            dst[dst_temp]     = src[src_temp + 1];
            dst[dst_temp + 1] = src[src_temp + 2];
            dst[dst_temp + 2] = src[src_temp + 3];
        }
    }
}

void g2d_mem2file(cyg_uint8 *buffer,int w,int h,int pitch,FILE *fp1,cyg_uint8 source_choice)
{
    cyg_uint8 OffsetBits = 54;
	cyg_uint8 buf[OffsetBits];
	cyg_uint8 offset = 0;
	int i = 0, j = 0;
	cyg_uint8 *pStart = NULL;
	unsigned int nSize = 0;
    cyg_uint32 temp = 0, ctemp = 0;
	if(((3 * w) % 4) != 0)
	 {offset = 4 - ((3 * w) % 4);}
	   
	cyg_uint32 InfoSize = (w * 3 + offset) * h * sizeof(cyg_uint8);
	cyg_uint32 Size = InfoSize + OffsetBits;
    cyg_uint8 bitperpixel = 24 ,plane = 1,bisize = 40;
	for(i = 0; i < OffsetBits; i++)
	{
	    buf[i]=0;
	}  
	buf[0]  = 0x42;
	buf[1]  = 0x4D;
	buf[2]  = (Size & 0xff);
	buf[3]  = ((Size >> 8) & 0xff);
	buf[4]  = ((Size >> 16) & 0xff);
	buf[5]  = ((Size >> 24) & 0xff);
	buf[10] = OffsetBits & 0xff;
	buf[11] = (OffsetBits >> 8) & 0xff;
	buf[12] = (OffsetBits >> 16) & 0xff;	 
	buf[13] = (OffsetBits >> 24) & 0xff;	 
	buf[18] = w & 0xff;
	buf[19] = (w >> 8) & 0xff;	 
	buf[20] = (w >> 16) & 0xff;
	buf[21] = (w >> 24) & 0xff;
	buf[22] = h & 0xff;
    buf[23] = (h >> 8) & 0xff;	 
	buf[24] = (h >> 16) & 0xff;
	buf[25] = (h >> 24) & 0xff;
	buf[26] = plane && 0xff;
	buf[27] = (plane >> 8) & 0xff;	  
	buf[28] = bitperpixel & 0xff;
	buf[29] = (bitperpixel >> 8) & 0xff;
	buf[34] = InfoSize & 0xff;
	buf[35] = (InfoSize >> 8) & 0xff;	 
	buf[36] = (InfoSize >> 16) & 0xff;
	buf[37] = (InfoSize >> 24) & 0xff;
	buf[17] = (bisize >> 24) & 0xff;
	buf[16] = (bisize >> 16) & 0xff;
	buf[15] = (bisize >> 8)&0xff;
	buf[14] = (bisize) & 0xff;
	if(fp1)
	{
	    fwrite (buf , sizeof(cyg_uint8) , 54 ,fp1 );
	}
	nSize = (w * 3 + offset) * h ;
	pStart = (cyg_uint8 *)malloc(nSize);
    pStart = (cyg_uint8 *)((cyg_uint32)pStart | 0x40000000);
    inverse_array(buffer,pitch,h);
    for (j = 0 ; j < h ; j++)
    {
        temp = j * (pitch + offset);  
        for (i = 0 ; i < pitch ; i += 3)
        {
            ctemp = temp + i; 
            pStart[ctemp]     = buffer[ctemp + 2];
            pStart[ctemp + 1] = buffer[ctemp + 1];
            pStart[ctemp + 2] = buffer[ctemp];
        }    
    }
	fwrite(pStart, 1, nSize, fp1);
    pStart = (cyg_uint8 *)((cyg_uint32)pStart & ~(0x40000000));
	free(pStart);
    pStart = NULL;
	fclose(fp1);
}

int gdecodejpeg(const char* infilename, const char* outfilename, hw_option_ptr setting)
{
    struct jpeg_decompress_struct cinfo = {0};
	struct jpeg_error_mgr jerr = {0};
	FILE * input_file = NULL;
  	JDIMENSION num_scanlines = 0;
  	djpeg_dest_ptr dest_mgr = NULL;
  	my_dest_ptr my_dest = NULL;
	int ret = 0;
	int usemydest = 1;
#ifdef __performance	
	wait_for_tick();
	t1 = cyg_current_time();
#endif // __performance
	if ((input_file = fopen(infilename, "rb")) == NULL) 
	{
      	diag_printf("can't open %s\n", infilename);
        return 0;
    }
	else
	{
		diag_printf("Input file : %s \n",infilename);
	}
    my_dest = (my_dest_ptr)malloc(sizeof(struct my_dest_struct) + 100);
    if (my_dest == NULL)
    {
  	    diag_printf("No enough memory!\n");
        return 0;
    }

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_CreateDecompress(&cinfo, JPEG_LIB_VERSION, sizeof(struct jpeg_decompress_struct));
	APprintf("[JPEG] 2.Specify data source for decompression\n");
#ifdef __performance
	t2 = cyg_current_time();
	t_ldfile = (t2-t1);
    t1 = cyg_current_time();
#endif // __performance
	jpeg_stdio_src(&cinfo, input_file);
	APprintf("[JPEG] 3.Read file header, set default decompression parameters\n");
  	jpeg_read_header(&cinfo, TRUE);// <-- incorrect pre_size in memory is here.11/12 QQ 
	APprintf("[JPEG] 4.Start decompressor\n");
  	jpeg_start_decompress(&cinfo);
  	/* Write output file header */
  	if (!usemydest)
  	{
  		//write bmp header
  		//dest_mgr = jinit_write_bmp(&cinfo, FALSE);	
	}
	if (my_dest && usemydest)
	{
		my_dest->nwidth = cinfo.output_width;
		my_dest->nheight = cinfo.output_height;
		my_dest->npitch = ((((cinfo.cur_comp_info[0]->width_in_blocks / 
		cinfo.cur_comp_info[0]->h_samp_factor) + 3) & ~3) + 2) * 16 * 4;
        //int allocsize = my_dest->npitch * ((my_dest->nheight + 15) & ~15);//jiunhau
        //my_dest->imagebits = NULL;
        //my_dest->imagebits = (char *)malloc_align(allocsize, 1024 * 4);
        //my_dest->imagebits = (char *)((int)my_dest->imagebits |0x40000000);
    }
#ifdef __performance
	t2 = cyg_current_time();
	t_swdecode = (t2 - t1);
#endif // __performance
    /* Process data */
	if (usemydest)
	{
	    APprintf("[JPEG] 5\n");
	/* Step 6: while (scan lines remain to be read) */
#ifdef __h_jpeg
#ifdef __performance
        t1 = cyg_current_time();
#endif // __performance
        j_hw_ptr hwset = (&cinfo)->phwset;
        
        hwset->down_sample_rate = setting->sample_rate;
        hwset->nSafeMode = setting->saving_mode;
        (setting->filter_enable == 0) ? (hwset->filter_used = false):(hwset->filter_used = true);
        
        ret = __sishwdecode_jpg(&cinfo, my_dest->imagebits, &show_buffer);
        #ifdef __performance
        t2 = cyg_current_time();
		t_hwdecode = (t2 - t1);
        #endif
        APprintf("HW_JPEG_end\n"); 
#else // __h_jpeg
		while (cinfo.output_scanline < cinfo.output_height) 
		{
			JSAMPROW rowptr[1];
			//rowptr[0] = (JSAMPROW)(my_dest->imagebits +cinfo.output_scanline * my_dest->npitch);
			jpeg_read_scanlines (&cinfo, rowptr, 1);
		}
#endif // __h_jpeg
	}
	else
	{
	/* while (scan lines remain to be read) */
		while (cinfo.output_scanline < cinfo.output_height) 
	    {
		    num_scanlines = jpeg_read_scanlines(&cinfo, dest_mgr->buffer,dest_mgr->buffer_height);
            (*dest_mgr->put_pixel_rows) (&cinfo, dest_mgr, num_scanlines);
		}
		(*dest_mgr->finish_output) (&cinfo, dest_mgr);
		(void) jpeg_finish_decompress(&cinfo);
	    APprintf("[JPEG] 7.Finish decompression\n");
	}
#ifdef __write_bmp
    cyg_uint8 *temp = NULL;
    FILE * output_file = NULL;
    if ((output_file = fopen(outfilename, "wb")) == NULL) 
    {
        diag_printf("can't open %s\n", outfilename);
    }
    else
    {
        diag_printf("Output file : %s \n",outfilename);
    }
    temp = (cyg_uint8 *)malloc(sizeof(cyg_uint8) * 3 * show_buffer.width * show_buffer.height);
    temp = (cyg_uint8 *)((cyg_uint32)temp | 0x40000000);
    diag_printf("Buffer create ok!\n");
    g2d_4to3(a, show_buffer.width , show_buffer.height, show_buffer.width << 2, temp);
    g2d_mem2file(temp, show_buffer.width, show_buffer.height, show_buffer.width * 3, output_file, 1);
    diag_printf("Write file over!\n");
    jpeg_free(temp);
#endif
	APprintf("[JPEG] 6\n");
	jpeg_destroy_decompress(&cinfo);
	APprintf("[JPEG] 8.Release JPEG decompression object\n");
#ifdef __performance
	show_test();
#endif // __performance
	if (input_file != stdin)
	{
    	fclose(input_file);
	}
    free(my_dest);
    //t1 = cyg_current_time();
    //Bilinear(a, 800, 480, 800, 800, b, 0.5, 0.5);
    //t2 = cyg_current_time();
    //t_swsca = (t2 - t1);
    //lcd_flip(1);
    //show_test();
    return 1;
}

int cyg_user_start( void )
{
	int err = 0;
    memset(thread_a_stack, 0, THREAD_STACK_SIZE);
	lcd_getinfo(&lcd_info_QQ);
	lcd_dev_test();
    g2d_dev_test();
	a = (cyg_uint8 *)lcd_info_QQ.fb1;
	b = (cyg_uint8 *)lcd_info_QQ.fb2;
    show_buffer.height = lcd_info_QQ.height;
    show_buffer.width = lcd_info_QQ.width;
    show_buffer.pitch = lcd_info_QQ.width;
    show_buffer.addr = a;
	diag_printf("frame buffer1 : %08x | frame buffer2 : %08x\n", a, b);  
	#if defined(CYGSEM_FILEIO_BLOCK_USAGE)
		struct cyg_fs_block_usage usage;
	#endif
    diag_printf("Prepare to mount SD disk on partition 0.\n");
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
	cyg_thread_create(
			THREAD_PRIORITY,
			thread_jpeg_decode,
			(cyg_addrword_t)75,
			"Thread JPEG",
			(void *)thread_a_stack,
			THREAD_STACK_SIZE,
		    &thread_a_hdl,
		    &thread_a_obj );	
	CYG_TEST_INIT();
    cyg_thread_resume(thread_a_hdl);	//Start it!
    cyg_scheduler_start();
	return 0;
}

void write_frame1(int w,int h,cyg_uint8 *dst,cyg_uint8 alpha,cyg_uint8 *src)
{
    int pitch = w << 2; 
	int pitch_s = w << 2;
	int i = 0, j = 0, k = 0;
	cyg_int32 temp = 0, temp1 = 0;

	for(j = 0; j < h; j++)
	{
		for(i = 0, k = 0; i < pitch; i += 4, k += 4)
		{
			temp  = j * pitch + i;
			temp1 = j * pitch_s + k;
			dst[temp]   = src[temp1];
			dst[temp+1] = src[temp1+1];
			dst[temp+2] = src[temp1+2];
			dst[temp+3] = src[temp1+3];
		}       
	}
}

void lag(int x) //0.0001sec
{
	int j = 0;
	cyg_uint32 i = 0;
	for(j = 0; j < x; j++)
	{
		for(i = 0; i < 200000000; i++)
		{
			asm("NOP");
		}
	}
}
#ifdef show_dir_list
static void dir_slide_show( char *name, int statp )
{
    int err = 0;
    DIR *dirp = NULL;
    hw_option_ptr jpeg_parameter = NULL;

    dirp = opendir( name );
    if( dirp == NULL ) SHOW_RESULT( opendir, -1 );

    for(;;)
    {
        struct dirent *entry = readdir( dirp );
        
        if( entry == NULL )
            break;
        
        if( statp )
        {
            char fullname[PATH_MAX];
            struct stat sbuf = {0};

            if( name[0] )
            {
                strcpy(fullname, name );
                if( !(name[0] == '/' && name[1] == 0 ) )
                    strcat(fullname, "/" );
            }
            else fullname[0] = 0;
            
            strcat(fullname, entry->d_name);
            
            if(strstr(fullname, ".jpg") || strstr(fullname, ".JPG"))
            {
                g2d_memset(a, 0, lcd_info_QQ.width, lcd_info_QQ.height, lcd_info_QQ.width);
                err = stat( fullname, &sbuf );
                
                jpeg_parameter->filter_enable = false;
                jpeg_parameter->sample_rate = 0;
                if(sbuf.st_size < 30000)
                {
                    jpeg_parameter->saving_mode = 0;
                }
                else
                {
                    jpeg_parameter->saving_mode = 1;
                }
                //diag_printf(" size : %d| saving mode : %d\n", sbuf.st_size,jpeg_parameter->saving_mode);
                gdecodejpeg(fullname, NULL, jpeg_parameter);
                lag_rc(1);
            }
            
            #if 0
            err = stat( fullname, &sbuf );
            if( err < 0 )
            {
                if( errno == ENOSYS )
                    diag_printf(" <no status available>");
                else SHOW_RESULT( stat, err );
            }
            else
            {
                //diag_printf(" [mode %08x ino %08x nlink %d size %ld]",
                //            sbuf.st_mode,sbuf.st_ino,sbuf.st_nlink,sbuf.st_size);
            }
            #endif
        }
    }
    err = closedir( dirp );
    if( err < 0 ) SHOW_RESULT( stat, err );
}
#endif

//CHAOBAN TEST
static void thread_jpeg_decode(cyg_addrword_t data)
{ 
#ifdef slice_view
    int i = 0;
    int file_number = 16;
    char input_file_name[30] = {0};
    char output_file_name[30] = {0};
    //char *temp1 = "/ab";    // small jpeg
    char *temp1 = "/WM";
	char *temp = (char *)malloc(sizeof(cyg_uint8) * 32);
    
#endif
    hw_option_ptr jpeg_parameter = NULL;
    int count = 0;
    jpeg_parameter->filter_enable = false;
    jpeg_parameter->sample_rate = 0;
    jpeg_parameter->saving_mode = 1;
    while(1)
	{
	    //g2d_memset(a,pixel(255,255,255,255),lcd_info_QQ.width,lcd_info_QQ.height,lcd_info_QQ.width);
#ifdef slice_view
#ifdef __write_bmp
	    for(i = 1; i < file_number; i++) 
	    {        
		   	strcpy(input_file_name, temp1);
		    int2str(i,temp);
		    strcat(input_file_name,temp);
            
            strcpy(output_file_name, input_file_name);
			strcat(input_file_name,".jpg");
            strcat(output_file_name,".bmp");
            gdecodejpeg(input_file_name,output_file_name);
	     }
#else
        //file_number = 23;
       	for(i = 1; i < file_number; i++) 
	    {
	        g2d_memset(a,pixel(255,255,255,255),lcd_info_QQ.width,lcd_info_QQ.height,lcd_info_QQ.width);
		   	strcpy(input_file_name, temp1);
		    int2str(i,temp);
		    strcat(input_file_name,temp);
            
			strcat(input_file_name,".jpg");
            gdecodejpeg(input_file_name,"/QQ.bmp", jpeg_parameter);
            //breakp(111);
            lag_rc(1); 
	    } 
#endif// ~__write_bmp
#endif// ~slice_view
//******************************************************************************
#ifdef show_dir_list
    dir_slide_show( "/", true );
#endif
//******************************************************************************
/* pre scaling & filter */
#ifdef verify_prescaling
        int j = 0, i = 0, k = 0;
        for(k = 0; k < 2; k++)
        {
            for(j = 0; j < 2; j++)
            {
                for(i = j; i < 4; i++)
                {
                    if(j == 1 && (i == 3)){continue;}
                    g2d_memset(a,pixel(255,255,255,255),lcd_info_QQ.width,lcd_info_QQ.height,lcd_info_QQ.width);
                    jpeg_parameter->filter_enable = j;
                    jpeg_parameter->sample_rate = i;
                    jpeg_parameter->saving_mode = k;
                    //jpeg_parameter->saving_mode = 1;
                    diag_printf("down_sample : %d | filter : %d | saving mode : %d\n", i, j, k);
                    gdecodejpeg("/TEST1.jpg","/010.bmp", jpeg_parameter);
                    lag_rc(1);
                }
            }
        }
#endif
//*******************************************************************************************
/* shift problem */
#ifdef verify_shift_problem
        jpeg_parameter->filter_enable = false;
        jpeg_parameter->sample_rate = 0;
        jpeg_parameter->saving_mode = 1;
        //gdecodejpeg("/8x6.JPG","/010.bmp", jpeg_parameter);
        gdecodejpeg("/x_02.jpg","/010.bmp", jpeg_parameter);
        //gdecodejpeg("/010.jpg","/010.bmp", jpeg_parameter);
        breakp(1000);        
#endif
//*******************************************************************************************
#ifdef slice_view

        jpeg_parameter->filter_enable = false;
        jpeg_parameter->sample_rate = 0;
        jpeg_parameter->saving_mode = 1;
        
        gdecodejpeg("/10MP_411.jpg","/010.bmp", jpeg_parameter);
        lag_rc(1);        
        gdecodejpeg("/10MP_422.jpg","/010.bmp", jpeg_parameter); 
        lag_rc(1);
        gdecodejpeg("/10MP_444.jpg","/010.bmp", jpeg_parameter);
        lag_rc(1);
        gdecodejpeg("/010.jpg","/010.bmp", jpeg_parameter); 
        lag_rc(1);
        gdecodejpeg("/TEST1.jpg","/010.bmp", jpeg_parameter);
        lag_rc(1);
        gdecodejpeg("/8x6.jpg","/010.bmp", jpeg_parameter);
        lag_rc(1);
        gdecodejpeg("/31x20.jpg","/010.bmp", jpeg_parameter);
        lag_rc(1);
        gdecodejpeg("/1000.jpg","/010.bmp", jpeg_parameter);     
        lag_rc(1);
        gdecodejpeg("/46x31.jpg","/010.bmp", jpeg_parameter); 
        lag_rc(1);
        gdecodejpeg("/39x48.jpg","/010.bmp", jpeg_parameter); 
        lag_rc(1);
        gdecodejpeg("/39x48_4.jpg","/010.bmp", jpeg_parameter); 
        lag_rc(1);

        /*
            /56x37.jpg
            /39x26.jpg
            /40x26.jpg
            /39x48.jpg   //20MP
        */
#endif
        count++;
		diag_printf("***Test count : %dth!!!!***\n", count);	
	}
}

void Bilinear(cyg_uint8 *src, int s_w, int s_h, int s_pitch, int d_pitch, cyg_uint8 *dst, float h_sca, float v_sca)
{
    float B_a,B_b,a,b,c,d; //alpha beta
    int i, j, d_h, d_w, s_x, s_y;
    cyg_uint32 src_total, dst_total, src_total_1;
    d_w=(int)(s_w * h_sca);
    d_h=(int)(s_h * v_sca); 

    for(j = 0; j < d_h; j++)
    {
     	for(i = 0; i < d_w; i++)
     	{		
			s_x=(int)(i / h_sca);
			s_y=(int)(j / v_sca);
			if(s_x >= (s_w - 1)){s_x = (s_w - 2);}
			if(s_y >= (s_h - 1)){s_y = (s_h - 2);}
			B_a=(i / h_sca) - s_x;
			B_b=(j / v_sca) - s_y;
					
			a = (1 - B_a) * (1 - B_b);
			b = B_a * (1 - B_b);
			c = (1 - B_a) * B_b;
			d = B_a * B_b;
							
			src_total=(s_y * s_pitch + s_x) << 2;
			dst_total=(j * d_pitch + i) << 2;
			src_total_1 = src_total + (s_w << 2);			
			dst[dst_total] =(int)
				(a*src[src_total]+b*src[src_total+4]+c*src[src_total_1]+d*src[src_total_1+4]); ;
			dst[dst_total+1]=(int)
				(a*src[src_total+1]+b*src[src_total+5]+c*src[src_total_1+1]+d*src[src_total_1+5]);          
			dst[dst_total+2]=(int)
				(a*src[src_total+2]+b*src[src_total+6]+c*src[src_total_1+2]+d*src[src_total_1+6]); 
			dst[dst_total+3]=(int)                                                                       
     	       	(a*src[src_total+3]+b*src[src_total+7]+c*src[src_total_1+3]+d*src[src_total_1+7]);	
	 	}
	}
}

void lag_rc(int x)
{ 
	int j = 0;
	cyg_uint32 i = 0;
	for(j = 0; j < x; j++)
	{
		for(i = 0; i < 8000000; i++)
		{
			asm("NOP");
		}
    }
}

