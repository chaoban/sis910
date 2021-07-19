/*
 * Image decode routine for JPEG files
 *
 * JPEG support must be enabled (see README.txt in contrib/jpeg)
 *
 *at GdDecodeJPEG(buffer_t * src, PMWIMAGEHDR pimage, PSD psd, MWBOOL fast_grayscale) 
 *if fast_grayscale bit0 = 1,it will decoder pure gray image(R=G=B, 0~255), 0 will be true color¡F
 *if fast_grayscale bit1 = 11,it will use fast IDCT to decoder, if 0 will use standard decoder
 *if fast_grayscale bit2 = 1, it will decoder thumbnail first, 
 *							  if find no thumbnail then decoder original image, if 0 then original image
 *----20008-04-29 shengwei----
 *
 * SOME FINE POINTS: (from libjpeg)
 * In the below code, we ignored the return value of jpeg_read_scanlines,
 * which is the number of scanlines actually read.  We could get away with
 * this because we asked for only one line at a time and we weren't using
 * a suspending data source.  See libjpeg.doc for more info.
 *
 * We cheated a bit by calling alloc_sarray() after jpeg_start_decompress();
 * we should have done it beforehand to ensure that the space would be
 * counted against the JPEG max_memory setting.  In some systems the above
 * code would risk an out-of-memory error.  However, in general we don't
 * know the output image dimensions before jpeg_start_decompress(), unless we
 * call jpeg_calc_output_dimensions().  See libjpeg.doc for more about this.
 *
 * Scanlines are returned in the same order as they appear in the JPEG file,
 * which is standardly top-to-bottom.  If you must emit data bottom-to-top,
 * you can use one of the virtual arrays provided by the JPEG memory manager
 * to invert the data.  See wrbmp.c for an example.
 *
 * As with compression, some operating modes may require temporary files.
 * On some systems you may need to set up a signal handler to ensure that
 * temporary files are deleted if the program is interrupted.  See libjpeg.doc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "device.h"
#include "swap.h"

#if MW_FEATURE_IMAGES && defined(HAVE_JPEG_SUPPORT)
#include <jpeg/jpeglib.h>
#define thbnlength 10240
static buffer_t *inptr;

//for thumbnail decode
static	unsigned char * buf;
static  buffer_t src1;
static	char e1_found;

static	volatile unsigned long pos=2;
static	volatile unsigned short length,begin,end;


static void
init_source(j_decompress_ptr cinfo)
{
	cinfo->src->next_input_byte = inptr->start;
	cinfo->src->bytes_in_buffer = inptr->size;
}

static void
fill_input_buffer(j_decompress_ptr cinfo)
{
	return;
}

static void
skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	if (num_bytes >= inptr->size)
		return;
	cinfo->src->next_input_byte += num_bytes;
	cinfo->src->bytes_in_buffer -= num_bytes;
}

static boolean
resync_to_restart(j_decompress_ptr cinfo, int desired)
{
	return jpeg_resync_to_restart(cinfo, desired);
}

static void
term_source(j_decompress_ptr cinfo)
{
	return;
}
/*****************
*fast_grayscale: 
*	bit0: =1,gray;  =0,color
*	bit1: =1,fastjpeg;  =0,floatjpeg decode
*	bit2: =1,thumbnail; =0,true_pic decode
******************/
#ifdef __wei_jpeg
static int jpeg_use_hw_decode = 1;
//int GdDecodeJPEG(buffer_t * src, PMWIMAGEHDR pimage, PSD psd, MWBOOL fast_grayscale)
int GdDecodeJPEG(char * path, PMWIMAGEHDR pimage, PSD psd, MWBOOL fast_grayscale)
{
    diag_printf("Jpeg decoder start\n");
	struct jpeg_decompress_struct cinfo = {0};
	struct jpeg_error_mgr jerr = {0};
	struct jpeg_source_mgr smgr = {0};
	display_info jpeg_output={0};
	FILE * input_file = NULL;
  	JDIMENSION num_scanlines = 0;
  	//djpeg_dest_ptr dest_mgr = NULL;  	
  	int* rgbdst = NULL;
	int ret=2;
	int usemydest = 1;
	int i = 0;
	if ((input_file = fopen(path, "rb")) == NULL) 
	{
      	diag_printf("can't open %s\n", path);
    }
	else
	{
		diag_printf("Input file : %s \n",path);

	}
	
	cinfo.err = jpeg_std_error(&jerr);
	//jpeg_create_decompress(&cinfo);
	jpeg_CreateDecompress(&cinfo, JPEG_LIB_VERSION,sizeof(struct jpeg_decompress_struct));
	
	//diag_printf("============2\n");
#if 0
	inptr=input_file;

	smgr.init_source = (void *) init_source;
	smgr.fill_input_buffer = (void *) fill_input_buffer;
	smgr.skip_input_data = (void *) skip_input_data;
	smgr.resync_to_restart = (void *) resync_to_restart;
	smgr.term_source = (void *) term_source;
	cinfo.src =&smgr;
	//inptr=input_file;
#endif
//	inptr = input_file;

	///* Step 2: Specify data source for decompression */
	jpeg_stdio_src(&cinfo, input_file);
	//diag_printf("============3\n");
  	/* Step 3: Read file header, set default decompression parameters */
  	jpeg_read_header(&cinfo, TRUE);
	//diag_printf("============4\n");
	/* Step 4: Start decompressor */
  	jpeg_start_decompress(&cinfo);
  	/* Write output file header */
  	
	
	{
		//my_dest->nwidth = cinfo.output_width;
		//my_dest->nheight = cinfo.output_height;
		//my_dest->npitch = (my_dest->nwidth*4 + 3) & ~3;
		//my_dest->imagebits = (char *)malloc(my_dest->npitch * my_dest->nheight);
		//my_dest->imagebits = (int)my_dest->imagebits |0x40000000;
		
		pimage->width = cinfo.output_width;
        pimage->height = cinfo.output_height;
        pimage->planes = 1;
        pimage->bytesperpixel = 4;
        pimage->pitch = 4 * pimage->width;
        pimage->compression = MWIMAGE_RGB | MWIMAGE_ALPHA_CHANNEL;//MWIMAGE_BGR;	/* RGB not BGR order*/
        pimage->palsize =  0;
        pimage->bpp = 32;	
        
        //pimage->imagebits = (int)pimage->imagebits |0x40000000;

	}
	/* Process data */
	
	{
	    //diag_printf("============5\n");
	    /* Step 6: while (scan lines remain to be read) */

        diag_printf("output_width %d, output_height %d\n",cinfo.output_width, cinfo.output_height);
        
        jpeg_output.pitch = pimage->width;
        jpeg_output.width = pimage->width;
        jpeg_output.height = pimage->height;
        jpeg_output.addr =  (char *)malloc_align(pimage->width * pimage->height * 4, 1024);
        jpeg_output.addr = (char *)((cyg_uint32)jpeg_output.addr |0x40000000);
        pimage->imagebits = jpeg_output.addr;        
        pimage->width = jpeg_output.width;
        pimage->height = jpeg_output.height;
        pimage->planes = 1;
        pimage->bytesperpixel = 4;
        pimage->pitch = 4 * jpeg_output.width;
        
		ret = __sishwdecode_jpg(&cinfo, NULL, &jpeg_output);
		diag_printf("Jpeg decoder end\n");

	}
	

	//diag_printf("============6\n");
	/* Step 7: Finish decompression */
  	
	/* Step 8: Release JPEG decompression object */
	//jpeg_destroy_decompress(&cinfo);
	diag_printf("Write framebuffer end\n");

	if (input_file != stdin)
	{
	    //diag_printf("close end\n");
    	fclose(input_file);
   	} 
	return 1;
}
#else //__wei_jpeg
int
GdDecodeJPEG(buffer_t * src, PMWIMAGEHDR pimage, PSD psd, MWBOOL fast_grayscale)
{
	int i;
	int ret = 2;		/* image load error */
	unsigned char magic[8];
	struct jpeg_source_mgr smgr;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int FASTJPEGinflag=0;

	if (fast_grayscale & 4)
	{
		pos=2;e1_found=0;
		buf=src->start;
		if((buf[0] !=0xff)||buf[1] !=0xD8) return 0;
		while(!e1_found && pos<thbnlength){
			if(buf[pos++] != 0xff) continue;
			switch(buf[pos++])
			{
			case 0xe1:
			case 0xed:
				length=(unsigned short)(buf[pos]);
				length=length<<8;
				length+=buf[pos+1];
				e1_found=1;
				end=pos+length;
				pos+=2;
FIND_FF:
				if(buf[pos] !=0xff & pos<thbnlength){pos++;goto FIND_FF;}
				pos++;
				if(buf[pos] !=0xd8 & pos<thbnlength){pos++;goto FIND_FF;}
				pos++;
				if(pos>=thbnlength)break;
				if(buf[pos] !=0xff)break;
				begin=pos-2;
				src1.start=src->start+begin;
				src1.size=end-begin;
				src1.offset=0;
				if(GdDecodeJPEG(&src1,pimage,psd,0)==1)
				return 1;
				break;
			case 0xFF:
				break;
			default:
				length=(unsigned short)(buf[pos]);
				length=length<<8;
				length+=buf[pos+1];
				pos+=length;
				break;
			}
		}
	}

	if (fast_grayscale & 2)
	{
		FASTJPEGinflag=1;
	}
	else
	{
		FASTJPEGinflag=0;
	}
#if FASTJPEGinflag
	extern MWPALENTRY mwstdpal8[256];
#else
	MWPALENTRY palette[256];
#endif

	/* first determine if JPEG file since decoder will error if not */
	GdImageBufferSeekTo(src, 0UL);
	if (GdImageBufferRead(src, magic, 2) != 2
	 || magic[0] != 0xFF || magic[1] != 0xD8)
		return 0;	/* not JPEG image */
#if 0//cut 08-04-29, to support more pic and thumbnail
	if (GdImageBufferRead(src, magic, 8) != 8
	 || (strncmp(magic+4, "JFIF", 4) != 0
          && strncmp(magic+4, "Exif", 4) != 0))
		return 0;	/* not JPEG image */
#endif
	GdImageBufferSeekTo(src, 0);
	pimage->imagebits = NULL;
	pimage->palette = NULL;

	/* Step 1: allocate and initialize JPEG decompression object */
	/* We set up the normal JPEG error routines. */
	cinfo.err = jpeg_std_error(&jerr);
	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);
	/* Step 2:  Setup the source manager */
	smgr.init_source = (void *) init_source;
	smgr.fill_input_buffer = (void *) fill_input_buffer;
	smgr.skip_input_data = (void *) skip_input_data;
	smgr.resync_to_restart = (void *) resync_to_restart;
	smgr.term_source = (void *) term_source;
	cinfo.src = &smgr;
	inptr = src;

	/* Step 2: specify data source (eg, a file) */
	/* jpeg_stdio_src (&cinfo, fp); */

	/* Step 3: read file parameters with jpeg_read_header() */
	jpeg_read_header(&cinfo, TRUE);
	/* Step 4: set parameters for decompression */
	cinfo.out_color_space = (fast_grayscale & 1)? JCS_GRAYSCALE: JCS_RGB;
	cinfo.quantize_colors = FALSE;
#if FASTJPEGinflag
	goto fastjpeg;
#endif
	if (!(fast_grayscale & 1)) {
		if (psd->pixtype == MWPF_PALETTE) {
fastjpeg:
			cinfo.quantize_colors = TRUE;
#if FASTJPEGinflag
			cinfo.actual_number_of_colors = 256;
#else
			/* Get system palette */
			cinfo.actual_number_of_colors = 
				GdGetPalette(psd, 0, psd->ncolors, palette);
#endif
	
			/* Allocate jpeg colormap space */
			cinfo.colormap = (*cinfo.mem->alloc_sarray)
				((j_common_ptr) &cinfo, JPOOL_IMAGE,
			       	(JDIMENSION)cinfo.actual_number_of_colors,
				(JDIMENSION)3);

			/* Set colormap from system palette */
			for(i = 0; i < cinfo.actual_number_of_colors; ++i) {
#if FASTJPEGinflag
				cinfo.colormap[0][i] = mwstdpal8[i].r;
				cinfo.colormap[1][i] = mwstdpal8[i].g;
				cinfo.colormap[2][i] = mwstdpal8[i].b;
#else
				cinfo.colormap[0][i] = palette[i].r;
				cinfo.colormap[1][i] = palette[i].g;
				cinfo.colormap[2][i] = palette[i].b;
#endif
			}
		}
	} else {
		/* Grayscale output asked */
		cinfo.quantize_colors = TRUE;
		cinfo.out_color_space = JCS_GRAYSCALE;
		cinfo.desired_number_of_colors = 256;
	}
	jpeg_calc_output_dimensions(&cinfo);
	pimage->width = cinfo.output_width;
	pimage->height = cinfo.output_height;
	pimage->planes = 1;
#if FASTJPEGinflag
	pimage->bpp = 8;
#else
	pimage->bpp = ((fast_grayscale & 1) || psd->pixtype == MWPF_PALETTE)?
		8: cinfo.output_components*8;
#endif
	GdComputeImagePitch(pimage->bpp, pimage->width, &pimage->pitch,
		&pimage->bytesperpixel);
	pimage->compression = MWIMAGE_RGB;	/* RGB not BGR order*/
	pimage->palsize = (pimage->bpp == 8)? 256: 0;
	pimage->imagebits = malloc(pimage->pitch * pimage->height);
	if(!pimage->imagebits)
		goto err;
	pimage->palette = NULL;
	if(pimage->bpp <= 8) {
		pimage->palette = malloc(256*sizeof(MWPALENTRY));
		if(!pimage->palette)
			goto err;
		if ((fast_grayscale & 1)) {
			for (i=0; i<256; ++i) {
				MWPALENTRY pe;
				/* FIXME could use static palette here*/
				pe.r = pe.g = pe.b = i;
				pimage->palette[i] = pe;
			}
		} else {
#if FASTJPEGinflag
			/* FASTJPEGinflag case only, normal uses hw palette*/
			for (i=0; i<256; ++i)
				pimage->palette[i] = mwstdpal8[i];
#endif
		}
	}
	/* Step 5: Start decompressor */
	jpeg_start_decompress (&cinfo);
	/* Step 6: while (scan lines remain to be read) */
	while(cinfo.output_scanline < cinfo.output_height) {
		JSAMPROW rowptr[1];
		rowptr[0] = (JSAMPROW)(pimage->imagebits +
			cinfo.output_scanline * pimage->pitch);
		jpeg_read_scanlines (&cinfo, rowptr, 1);
	}
	ret = 1;

err:
	/* Step 7: Finish decompression */
	jpeg_finish_decompress (&cinfo);
	/* Step 8: Release JPEG decompression object */
	jpeg_destroy_decompress (&cinfo);
	/* May want to check to see whether any corrupt-data
	 * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	 */
	return ret;
}
#endif //__wei_jpeg
#endif /* MW_FEATURE_IMAGES && defined(HAVE_JPEG_SUPPORT)*/

void generate_bmpfile(char *buffer,int w,int h,int pitch,FILE *fp1,cyg_uint8 source_choice)
{
	 cyg_uint8 OffsetBits=54;
	 cyg_uint8 buf[OffsetBits];
	 cyg_uint8 offset=0;
	 int i,j;
	 char *pStart, *pNew, *buftmp;
	 unsigned int nSize = 0;
	 int y =0, x =0;
	 if(((3*w)%4)!=0)
	  {offset=4-((3*w)%4);}
	   
	 cyg_uint32 InfoSize=(w+offset)*h*3*sizeof(cyg_uint8);
	 cyg_uint32 Size=InfoSize+OffsetBits;
	 cyg_uint8 bitperpixel=24,plane=1,bisize=40;
	 for(i=0;i<OffsetBits;i++)
	 {
		buf[i]=0;
	 }
	   
	   buf[0]=0x42;
	   buf[1]=0x4D;
	   buf[2]=(Size&0xff);
	   buf[3]=((Size>>8)&0xff);
	   buf[4]=((Size>>16)&0xff);
	   buf[5]=((Size>>24)&0xff);
	   buf[10]=OffsetBits&0xff;
	   buf[11]=(OffsetBits>>8)&0xff;
	   buf[12]=(OffsetBits>>16)&0xff;	 
	   buf[13]=(OffsetBits>>24)&0xff;	 
	   buf[18]=w&0xff;
	   buf[19]=(w>>8)&0xff;	 
	   buf[20]=(w>>16)&0xff;
	   buf[21]=(w>>24)&0xff;
	   buf[22]=h&0xff;
	   buf[23]=(h>>8)&0xff;	 
	   buf[24]=(h>>16)&0xff;
	   buf[25]=(h>>24)&0xff;
	   buf[26]=plane&&0xff;
	   buf[27]=(plane>>8)&0xff;	  
	   buf[28]=bitperpixel&0xff;
	   buf[29]=(bitperpixel>>8)&0xff;
	   buf[34]=InfoSize&0xff;
	   buf[35]=(InfoSize>>8)&0xff;	 
	   buf[36]=(InfoSize>>16)&0xff;
	   buf[37]=(InfoSize>>24)&0xff;
	   buf[17]=(bisize>>24)&0xff;
	   buf[16]=(bisize>>16)&0xff;
	   buf[15]=(bisize>>8)&0xff;
	   buf[14]=(bisize)&0xff;
	   if(fp1)
	   {
	   	fwrite (buf , sizeof(cyg_uint8) , 54 ,fp1 );
	   }
	   nSize = (w + offset) * h * 3;
	   pStart = pNew =(char *)malloc(nSize);
	   buftmp = (char *)buffer;
	   if(source_choice==0) //from bmp.h 
	   {
		   for (y = h - 1 ; y >= 0 ; y --)
		   {
				buffer = buftmp + (y * pitch);
				for (x = 0 ; x < w ; x++)
				{
					pNew[0] = buffer[0];
					pNew[1] = buffer[1];
					pNew[2] = buffer[2];
					pNew += 3;
					buffer += 3;
				}
				for (j = 0; j < offset; j++)
				{
					pNew[j] = 0x00;
		 		}
		   	}
			pNew += offset;
		}
		else                 //from HD -> 6B decoder
		{
			for (y = h - 1 ; y >= 0 ; y --)
		    {
				buffer = buftmp + (y * pitch);
				for (x = 0 ; x < w ; x++)
				{
					pNew[0] = buffer[2];
					pNew[1] = buffer[1];
					pNew[2] = buffer[0];
					pNew += 3;
					buffer += 3;
				}
				for (j = 0; j < offset; j++)
				{
					pNew[j] = 0x00;
				}
			}
			pNew += offset;
		}
	
	
	fwrite(pStart, 1, nSize, fp1);
	free(pStart);
	fclose(fp1);
	
}