/* update note

980223 - update the gateway size
980225 - separate the *.c and *.h 
         fixed thr Q_table size.
980227 - Add the macro sync
         Fixed the shift problem.
980306 - Add the pre-scaling path. 
980309 - Add the filter
980312 - Fixed the jpeg_output buffer error
980313 - Refine JPEG driver
*/
#include <assert.h>
#include <cyg/hal/hal_io.h> 
#include <jpeg/jinclude.h>
#include <jpeg/jpeglib.h>
#include <jpeg/sishwjpg.h>
#include <jpeg/jdhuff.h>
#include <jpeg/jdebug.h>    // This is for JPEG debug purpose
#include <cyg/io/display/g2d/G2D_io.h>
#include <memmanage/memmanage.h>
#include <pkgconf/jhwconfig.h>
#include <pkgconf/system.h>
//=================================================================

//#define DBG_message 1
//#define DBG_memory_dump 1

#ifdef DBG_message
#define driver_message(x)  diag_printf(x)
#else
#define driver_message(x)
#endif// ~DBG_message

#define HAL_Value_Read_UINT32(_reg_)  *((volatile CYG_WORD32 *)(_reg_))

extern int g2d_memcpy(cyg_uint8 *dst, cyg_uint8 *src, int obj_width, 
               int obj_height, int src_width, int dst_width);
extern int g2d_memset(cyg_uint8 * dst, int val, int obj_width, int obj_height, int dst_width);
extern int g2d_memscl(bool flag, cyg_uint32 dst_frmb, cyg_uint32 src_frmb, 
               cyg_uint32 src_x, cyg_uint32 src_y, cyg_uint32 dst_x, cyg_uint32 dst_y,
               cyg_uint32 vsr_i, cyg_uint32 vsr_n, cyg_uint32 vsr_m,
               cyg_uint32 hsr_i, cyg_uint32 hsr_n, cyg_uint32 hsr_m,
               int obj_width, int obj_height, int src_width, int dst_width);
extern void *malloc_align( size_t /* size */, size_t /* alignment */);
extern char Getch(void);//chaoban test

void jpeg_reset(cyg_uint32, cyg_uint32, cyg_uint32);
void JPEG_read(void);

inline void* jpeg_malloc(cyg_uint32, cyg_uint32, bool);
inline void jpeg_free(void *);

unsigned int hwformat[6] = {0x0, JPG_HW_YUV411_VAL, JPG_HW_YUV422_VAL,
                                 JPG_HW_YUV444_VAL, JPG_HW_YUV422V_VAL, 
                                 JPG_HW_YUV400_VAL}; 
typedef struct {
  struct jpeg_source_mgr pub;	/* public fields */
  FILE * infile;		/* source stream */
  JOCTET * buffer;		/* start of buffer */
  boolean start_of_file;	/* have we gotten any data yet? */
} sis_source_mgr;
    
typedef sis_source_mgr * sis_src_ptr;

inline void* 
jpeg_alloc(j_decompress_ptr cinfo, size_t sizeofobject, size_t ALIGN_TYPE)
{
    size_t odd_bytes = 0;
    //int slop = 100000;
    char *data_ptr = NULL;
    //Round up the requested size to a multiple of SIZEOF(ALIGN_TYPE)
    odd_bytes = sizeofobject % ALIGN_TYPE;
    if (odd_bytes > 0)
    {
        sizeofobject += ALIGN_TYPE - odd_bytes;
    }
    // Try to get space, if fail reduce slop and try again

    while (data_ptr == NULL)
    {
        data_ptr = (char *)((*cinfo->mem->alloc_small) ((j_common_ptr)cinfo, 
                   JPOOL_PERMANENT, sizeofobject));        
    }
#if 0
    while(1) 
    {
        //data_ptr = (char *)((*cinfo->mem->alloc_small) (cinfo, JPOOL_PERMANENT, sizeofobject));
        if (data_ptr != NULL)
	    {
	        break;
	    }
	    else
	    {
	        stall100;
	        slop -= 2;
	    }
        if (slop < MIN_SLOP)	/* give up when it gets real small */
        {
            printf("Try Alloc Error!!!\n");
        }
    }
#endif
    return (void *) data_ptr;
}

inline void* jpeg_malloc(cyg_uint32 size, cyg_uint32 alignment, bool noncache)
{
    cyg_uint8 *buffer_pointer = NULL;

    buffer_pointer = (cyg_uint8 *)malloc_align(size, alignment);

    //diag_printf("buffer_pointer : 0x%08x\n", buffer_pointer);
    while(buffer_pointer == NULL)
    {        
        buffer_pointer = (cyg_uint8 *)malloc_align(size, alignment);
    }
    #if 0
    if (buffer_pointer == NULL)
    {
        // Error!!
        driver_message("Error!! We cannot alloc memory.\n");
        return NULL;
    }    

    if (noncache)
    {
        buffer_pointer = (cyg_uint8 *)((cyg_uint32)buffer_pointer | 0x40000000);
    }
    #endif
    return (void *)buffer_pointer;
}

inline void jpeg_free(void *object)
{   
    //object = (void *)(((cyg_uint32)object) & (~0x40000000));
    free(object);
    object = NULL;
}

inline int __getbitstream(j_decompress_ptr cinfo)
{
    j_hw_ptr hwset = (j_hw_ptr)cinfo->phwset;
    sis_src_ptr src = (sis_src_ptr)cinfo->src;
    register const JOCTET * next_input_byte = cinfo->src->next_input_byte; 
	register size_t bytes_in_buffer = cinfo->src->bytes_in_buffer;
    cyg_uint16 offset = ftell(src->infile);// rember current pos "int"980204 jiunhau 
    cyg_uint32 fileLength = 0, readfilesize = 0;

    driver_message("__getbitstream\n");
    if (fseek(src->infile, 0, SEEK_END))
    {
        //File seek error.
        return -1;
    }
    fileLength = ftell(src->infile);        
    fseek(src->infile, offset, SEEK_SET);  
    readfilesize = fileLength - offset;     
    fileLength = readfilesize + bytes_in_buffer;
    if ((fileLength & 0x3) != 0)
    {
        fileLength = ((fileLength + 0x3) & ~(0x3));
    }
    hwset->srcbitstream.nsize = fileLength;    
    hwset->srcbitstream.base  = jpeg_alloc(cinfo, fileLength + 0x3FL, 8);
    hwset->srcbitstream.base  = (void *)(((cyg_uint32)hwset->srcbitstream.base + 0x3FL)&~0x3FL);
    
    memcpy(hwset->srcbitstream.base, next_input_byte, bytes_in_buffer);
    fread((void *)((cyg_uint32)(hwset->srcbitstream.base) + bytes_in_buffer), 1, readfilesize, src->infile);
    R16_CACHE_SYNC(hwset->srcbitstream.base, hwset->srcbitstream.nsize);
    
#ifdef DBG_memory_dump
    diag_printf("[BIT STREAM]========================\n");
    diag_dump_buf_with_offset(hwset->srcbitstream.base, hwset->srcbitstream.nsize, 0);
#endif //__DBG_BITSTREAM
	fseek (src->infile, offset, SEEK_SET); 
    return 1; 
}

inline int __getqmatrix(j_decompress_ptr cinfo)
{ 
   //bitstream is 16bits per element and size if by byte
   j_hw_ptr hwset = (j_hw_ptr)cinfo->phwset;
   int i = 0, j = 0;
   unsigned int nsize = 0;
   char *tmp_ptr = NULL;
   cyg_uint32 qtable_size = JPG_QUANT_TABLE_NSIZE;
   hwset->qmatrix.base = (void *)jpeg_alloc(cinfo, qtable_size + 0x3FL, 8);
   hwset->qmatrix.base = (void *)(((cyg_uint32)hwset->qmatrix.base + 0x3FL) & (~0x3FL));// 8byte align
   
   for (i = 0; i < HW_NUM_QUANT_TBLS; i++)
   {
        if (cinfo->quant_tbl_ptrs[i])
        {
		    tmp_ptr = (char *)((cyg_uint32)hwset->qmatrix.base + (nsize * 64));
            for (j = 0; j < 64; j++)
		    {
			    tmp_ptr[j] = (char)cinfo->quant_tbl_ptrs[i]->quantval[j];
		    }
		    nsize++;   
        }
    }
    hwset->qmatrix.nsize = JPG_QUANT_TABLE_NSIZE;
    R16_CACHE_SYNC(hwset->qmatrix.base, hwset->qmatrix.nsize);
#ifdef DBG_memory_dump
    diag_printf("[Q_TABLE]========================\n");
    diag_dump_buf_with_offset(hwset->qmatrix.base, hwset->qmatrix.nsize, 0);
#endif
    return 1; 
}

inline int __encodecanonic(j_decompress_ptr cinfo, boolean isDC, int tblno)
{
    int arrayoffset = 0;
    int ret = 0;
    j_hw_ptr hwset = cinfo->phwset;
    cyg_uint16 *dstarray = NULL;
    JHUFF_TBL *htbl = NULL;
    int p = 0, i = 0, l = 0, si = 0, numsymbols = 0;
    char huffsize[257] = {0};
    unsigned int huffcode[257] = {0};
    unsigned int code = 0;
    int j = 0;
    int start[17] = {0};
    int used_item = 0;
    // Note that huffsize[] and huffcode[] are filled in code-length order,
    // paralleling the order of the symbols themselves in htbl->huffval[].    
    // Find the input Huffman table
    if (tblno < 0 || tblno >= NUM_HUFF_TBLS)
    {
        ret = 0;
    }
    htbl = isDC ? cinfo->dc_huff_tbl_ptrs[tblno] : cinfo->ac_huff_tbl_ptrs[tblno];
    if (htbl == NULL)
    {
        ret = 0;   
    }
    
    // Figure C.1: make table of Huffman code length for each symbol
    //driver_message("read huf table, %s\n", (isDC?"DC":"AC"));
    p = 0;
    
    for (l = 1; l <= 16; l++) 
    {
        i = (int) htbl->bits[l];
        if (i < 0 || (p + i > 256))	// protect against table overrun
        return 0;
        while (i--)
        huffsize[p++] = (char) l;
    }
    huffsize[p] = 0;
    numsymbols = p;
    // Figure C.2: generate the codes themselves 
    // We also validate that the counts represent a legal Huffman code tree.
  
    code = 0;
    si = huffsize[0];
    p = 0;
    while (huffsize[p]) 
    {
        while (((int) huffsize[p]) == si)
        {
            huffcode[p++] = code;
            code++;
        }
        // code is now 1 more than the last code used for codelength si; but
        // it must still fit in si bits, since no code is allowed to be all ones.
        //
        if (((INT32) code) >= (((INT32) 1) << si))
        return 0;
        code <<= 1;
        si++;
    }
    
    switch (tblno)
    {
        case 0:
            arrayoffset = ( isDC ) ? 0 : 24;
            break;
        case 1:
            arrayoffset = ( isDC ) ? 12 : 74;
            break;
        default:
            assert (0);
    }
    dstarray = (cyg_uint16 *)((cyg_uint32)(hwset->hopper.base) + arrayoffset * sizeof(int));
    start[0] = 0;
    for ( i = 1; i <= 16 ; i++ )
    {
	    start[i] = ( start[i-1] + htbl->bits[i-1] ) << 1;	
    }
    
    /***************************************************
    *W.T.Generate the Start List here
    * change any code here means start_list_size[] also need to be updated
    ***************************************************/
    for (i = 1; i <= 12; i++)
    {
    	if (htbl->bits[i])
    	{
    	    /*to save the first item if we can deduce it*/
		    if (i < 12 && htbl->bits[i+1] && start[i] == 0)
		    {
			    assert(used_item==0);
			    continue;
		    }
            *(dstarray + used_item) = (start [i] << 3) | i;
            used_item++;
    	}
    }
    
    j = MAX_DEPTH;
    if (j > 12) /*j is the maximum codeword length*/
    {   
    	/*We've got long start codes*/
    	/*In this branch,
    	1. We always put 12 there, so that hardware knows to switch to long code.
    	2. The last code is always with bit 0 set and most importantly valid, i.e. not a dummy item
    	*/
    	if (!htbl->bits[12]) 
    	{
    		*(dstarray + used_item) = (start [12] << 3) | 12;
            used_item++;
    	}

    	for (i = 13; i <= MAX_DEPTH; i++) 
    	{
    		*(dstarray + used_item) = start[i] | ( i==j ? 1 : 0 );
    		used_item++;
    	}
    }
    else
    {
    	/*no long Start List code any more*/
    	/*In this branch,we 
    	1. EITHER don't have a start code at 12, the last item is 0xffff,  so the hardware knows to stop and
    	not to switch to a long code
    	2. OR we DO have a start code at 12.
    
    	So basically we have to know if code 12 is the last one or not!
    	*/

    	*(dstarray + used_item) = 0xffff; 
        used_item++;
    }
    if (used_item & 1)
    {
 	    *(dstarray + used_item) = 0x0000;
    	used_item++;    
    }
    if (used_item < 16) 
    {
	    for(i = used_item; i < 16; i += 2)
		{
			used_item += 2;
		}
    }
    int ceil_numsymbols = ( numsymbols + 3 ) / 4 * 4;
    p = 0;
    short tmp = 0;
    while (p < ceil_numsymbols)
    {     
      if (p&1)
      {
        tmp |= (( p < numsymbols ? htbl->huffval[p] : 0 ) & 0xFF);
        *(dstarray + used_item) = tmp;
        tmp = 0;
        used_item++;
      }
      else
      {
        tmp = (( p < numsymbols ? htbl->huffval[p] : 0 ) & 0xFF) << 8;
      }
      p++;
    }
    if (tmp!=0)
    {
        *(dstarray + used_item) = tmp;
    }
    ret = 1;
    return ret;
}

inline int __gethopper(j_decompress_ptr cinfo)
{
   //bitstream is 32bits base
   //Arrange is YDC->uvDC->YAC->uvAC
   //and bit is canonic
   //what is cannonic? it is diffucult to say><
   
	int ret = 0;
	j_hw_ptr hwset = cinfo->phwset;
    hwset->hopper.nsize = JPG_HOPPER_TABLE_NSIZE;
	hwset->hopper.base = jpeg_alloc(cinfo, hwset->hopper.nsize + 0x3FL, 8);
	hwset->hopper.base = (void *)(((cyg_uint32)(hwset->hopper.base) + 0x3FL) & (~0x3FL));//8byte align
    
    ret = __encodecanonic(cinfo, TRUE, 0);  
	ret = __encodecanonic(cinfo, TRUE, 1);    
	ret = __encodecanonic(cinfo, FALSE, 0);
    ret = __encodecanonic(cinfo, FALSE, 1);
    R16_CACHE_SYNC(hwset->hopper.base, hwset->hopper.nsize);
    
#ifdef DBG_memory_dump
    diag_printf("[Huffman Table]========================\n");
    diag_dump_buf_with_offset(hwset->hopper.base, hwset->hopper.nsize, 0);
#endif   
   return ret;
}

inline boolean checksize(uint width, uint height)
{
    uint size = 0, nwb = 0, nhb = 0;
    nwb = jdiv_round_up((long)width, 64L) * 64L * 4;//width pitch
    nhb = jdiv_round_up((long)height, 64L) * 64L; //height 
    size = nwb * nhb;
    //check if 2k *2k limit
    return (nwb > 512)?(true):(false);
}

inline J_Sample_Fmt selctfmt(j_decompress_ptr cinfo)
{
    J_Sample_Fmt efmt = JFmt_Unknow;
    jpeg_component_info* compptr = cinfo->cur_comp_info[0];
    if ( cinfo->num_components == 1 )
    {
        efmt = JFmt_YUV400;
    }
    else if ( compptr->h_samp_factor == 2 && compptr->v_samp_factor == 2 )
    {
        efmt = JFmt_YUV411;
    } 
    else if ( compptr->h_samp_factor == 2 && compptr->v_samp_factor == 1 )
    {
        efmt = JFmt_YUV422;
    }
    else if ( compptr->h_samp_factor == 1 && compptr->v_samp_factor == 1 )
    {
        efmt = JFmt_YUV444;
    }
    else if ( compptr->h_samp_factor == 1 && compptr->v_samp_factor == 2 )
    {
        efmt = JFmt_YUV422v; 
    }
    return efmt;
}

inline int Raster2TwoDAddr_Y( int raster_x, int raster_y, int dma_nhblk )
{
    int blk_num = 0, delta_x = 0, delta_y = 0, pos = 0;  
    blk_num = ( (raster_x >> 6) + ((raster_y >> 4) * dma_nhblk));
    blk_num = (dma_nhblk & 3) ? blk_num : (blk_num ^ ((raster_y & 0x10) >> 3) );
    delta_x = (raster_x & 0x3f);
    delta_y = (raster_y & 0x0f);
    pos = (blk_num << 10) | (delta_y << 6) | delta_x;
    return pos;
}

#ifdef __JPG_HW_BITBLT
GLOBAL(int) jpgHW_BitBlt(int *dst, int *src, int img_width, int nheight, int mcu_width, int hcomp)
{
    //although, the name of blt function is the same
    //we use hw bitblt function to do blt funtion
    cyg_uint8* TILE_FB  = (cyg_uint8*)src;
    cyg_uint8* LINEAR_FB = (cyg_uint8*)dst;
    int ldx = 0, ldy = 0, pos = 0;
    int ndma_nhblk = (mcu_width + 3) >> 2;
    for (ldy = 0; ldy < nheight / hcomp; ldy++)
    {
        LINEAR_FB = (char *)((cyg_uint32)dst + ldy * hcomp << 2 * img_width); //dst
        //for each mcu block
        for (ldx = 0; ldx < (img_width >> 4); ldx++)
        {
            pos = Raster2TwoDAddr_Y ((ldx << 6), ldy * hcomp, ndma_nhblk << 2);
            TILE_FB =  (cyg_uint8 *)((cyg_uint32)src + pos); //src
            g2d_memcpy((cyg_uint8 *)LINEAR_FB, (cyg_uint8 *)TILE_FB, 16, hcomp, 16, img_width);
            LINEAR_FB += 64;
        }
    }
    return 1;
}
#endif //__JPG_HW_BITBLT

#ifdef __JPG_R16_BITBLT
GLOBAL(int) R16_BitBlt(cyg_uint8 *dst,cyg_uint8 *src, int img_width, int img_height, int mcu_width)
{
    cyg_uint32 TILE_FB   = (cyg_uint32)src;
    cyg_uint32 LINEAR_FB = (cyg_uint32)dst|0x40000000;
    int ldx = 0, ldy = 0, col = 0, i = 0, j = 0, pos = 0, rgb_pos = 0;
    int rgb = 0;
    int ndma_nhblk = (mcu_width + 3) >> 2;
    
    for (ldy = 0; ldy < img_height; ldy = ldy + 1 )
    {
	    col = 0;
    	for (ldx = 0; ldx < mcu_width; ldx = ldx + 1 )
    	{	
    	    //printf( "[%04d, %04d]\n", ldx*8, ldy );      
    	    pos = Raster2TwoDAddr_Y (ldx << 6, ldy, ndma_nhblk << 2);
    	    //printf( "[%04d, %04d, %4d]\n", ldx, ldy,pos );
    	    for ( j = 0 ; j < 2 ; j ++ )
    	    {
    		    for ( i = 0 ; i < 8 ; i ++ )
    		    {
    		        if ( col < img_width )
    		        {
    			        rgb = ( col + i < img_width ) ? 
                              iior( TILE_FB + pos + ( i + j << 3 ) << 2 ) : 
                              0xffff0000;
    			        iiow ( LINEAR_FB + rgb_pos << 2, rgb );
    			        rgb_pos ++;
    		        }
    		    }
    		    col = col + 8;
    	    }
    	}
    }
#ifdef __DBG_LOG
    printf ( "R16 BitBlt Complete\n" );
#endif
    return 0;
}
#endif //__JPG_R16_BITBLT

LOCAL(void) loadsram(uint *src, uint length, uint ld_dma_base, uint ld_16)
{
    int i = 0;
    HAL_WRITE_UINT32(gw_lpc7, (cyg_uint32)src);  //0x151c, lpc chanel 7
    i = (ld_16) ? 0x00040000 : 0x0;
    HAL_WRITE_UINT32(0x90002630, i | ld_dma_base);
    HAL_WRITE_UINT32(0x90002620, length << 16 | 0x80000001);
    do //make sure current ld_dma is done
    {
        HAL_READ_UINT32(0x90002620, i);
    } while (i < 0); 
}

LOCAL(int) __sisfilljpegdata(j_decompress_ptr cinfo, display_info* display_buffer)
{
    //step1. fill hwsetting, including bitstream, hopper and qmatrix
    //fill the jpeg_hw_struct
    int ret = 0;
    j_hw_ptr hwset = cinfo->phwset;
    jpeg_component_info *compptr = cinfo->comp_info;

    driver_message("__sisfilljpegdata!!!\n");
    ret = __getbitstream(cinfo);

    if (ret != 1)
    {
        driver_message("__getbitstream error!!!\n");
    }
    ret = __getqmatrix(cinfo);
    if (ret != 1)
    {
        driver_message("__getqmatrix error!!!\n");
    }
    ret = __gethopper(cinfo); 
    if (ret != 1)
    {
        driver_message("__gethopper error!!!\n");
    }

    //2.mcu info
    hwset->nMcuwidth = compptr->width_in_blocks / compptr->h_samp_factor;
    hwset->nMcuwidth = ((compptr->width_in_blocks % compptr->h_samp_factor) != 0) ? 
                       (hwset->nMcuwidth + 1) : (hwset->nMcuwidth); 
    //diag_printf("compptr->width_in_blocks : %d\n", compptr->width_in_blocks);
    hwset->nMcuheight = compptr->height_in_blocks / compptr->v_samp_factor;
    hwset->nMcuheight = ((compptr->height_in_blocks % compptr->v_samp_factor) != 0) ?
                        (hwset->nMcuheight + 1) : (hwset->nMcuheight);
    //diag_printf("hwset->nMcuheight : %d | hwset->nMcuwidth : %d\n", hwset->nMcuheight, hwset->nMcuwidth);
    hwset->nblock_height = compptr->v_samp_factor * DCTSIZE;//forsafemode used
    hwset->nblock_width  = compptr->h_samp_factor * DCTSIZE;
    hwset->ninterval     = cinfo->restart_interval;
    hwset->nleading      = ((cyg_uint32)hwset->srcbitstream.base) & 0x1F;
    hwset->nbloknum = ((hwset->nMcuwidth & 0x3) == 0) ? 
                      (hwset->nMcuwidth + 2) : (hwset->nMcuwidth);
    hwset->eSampleFmt = selctfmt(cinfo);
    hwset->bdma_8x = checksize(cinfo->image_width, cinfo->image_height);

    // saving mode setting
    if(cinfo->output_height > display_buffer->height || 
       cinfo->output_width > display_buffer->width)
    {
        hwset->nSafeMode = 1;
    }
    else
    {
        hwset->nSafeMode = 0;
    }
    
#ifdef DBG_message
    switch(hwset->eSampleFmt)
    {
        case 1:
            diag_printf("JPEG Format  : 411\n");
            break;
        case 2:
            diag_printf("JPEG Format  : 422\n");
            break;
        case 3:
            diag_printf("JPEG Format  : 444\n");
            break;
        case 4:
            diag_printf("JPEG Format  : 422V\n");
            break;
    }
    EPRINTF(("BLOCK_WIDTH  : %d\n", hwset->nblock_width,));
    EPRINTF(("BLOCK_HEIGHT : %d\n", hwset->nblock_height));
    EPRINTF(("MB_RESTART   : %d\n", hwset->ninterval));
    EPRINTF(("HOR_BLOCK_#  : %d\n", hwset->nMcuwidth));
    EPRINTF(("VER_BLOCK_#  : %d\n", hwset->nMcuheight));
    EPRINTF(("IMAGE_WIDTH  : %d\n", cinfo->image_width));
    EPRINTF(("IMAGE_HEIGHT : %d\n", cinfo->image_height));
    EPRINTF(("STREAM_BASE  : %d\n", hwset->srcbitstream.base));
    EPRINTF(("H_BLOCK_NUM  : %d\n", hwset->nbloknum));
    EPRINTF(("Sample_FMT   : %d\n", hwset->eSampleFmt));
    EPRINTF(("BDMA_8X      : %d\n", hwset->bdma_8x)); 
#endif // ~DBG_message

    return ret;
}

LOCAL(int) __sisloadjpegdata(j_decompress_ptr cinfo)
{
    //step2. load to hw DMA, include bitstream, hopper and qmatrix
    j_hw_ptr hwset = cinfo->phwset;
    cyg_uint32 m = 0, n = 0, data_size = 0;
    //diag_printf("Bit stream size : %d\n", hwset->srcbitstream.nsize);
    m = hwset->srcbitstream.nsize >> 16;
    n = hwset->srcbitstream.nsize % (32 * 2048); // 32 delx uint | 2048 dely size 
    data_size = (m == 0) ? (1) : (m);
    data_size = (n == 0) ? (data_size) : (data_size + 1);
    driver_message("__sisloadjpegdata!!!\n");
    HAL_WRITE_UINT32(JPG_Enable, 0x4);//0x2680, reset
    driver_message("VDEC reset!!!\n"); 
    HAL_WRITE_UINT32(gw_reset, 0x100);
    HAL_WRITE_UINT32(gw_rff_val0, 0x08000808);//gw read fifo base and size
    HAL_WRITE_UINT32(gw_rd0_init0, GATEWAY_delx(data_size));
    HAL_WRITE_UINT32(gw_rd0_init1, (cyg_uint32)hwset->srcbitstream.base);//base
    HAL_WRITE_UINT32(gw_lpc0, 0x00300000);
    HAL_WRITE_UINT32(gw_lpc1, 0x00380000);
    HAL_WRITE_UINT32(gw_lpc2, 0x00340000);
    driver_message("gateway setup!!!\n");
    loadsram(hwset->hopper.base, 0x0200, 0x2000, 0x0); //load hopper    //hwset->hopper.nsize
    loadsram(hwset->qmatrix.base, 0x0080, 0x3c00, 0x1);//load Qmatrix   //hwset->qmatrix.nsize
    driver_message("Huffman load!!!\n");   
    return 1;
}

LOCAL(int) __sissetjpegdata(j_decompress_ptr cinfo, cyg_uint8 *jpeg_output)
{
    //step3. other hw setting
    cyg_uint32 dwset0 = 0, dwsafemode = 0;
    j_hw_ptr hwset = cinfo->phwset;
    driver_message("Hwjpeg setup!!!\n");
    dwset0 |= JPG_DMA_8X_ENABLE((hwset->bdma_8x) ? (0x1) : (0x0)) | hwformat[hwset->eSampleFmt];
    dwset0 |= JPG_DMA_RQ_LVL(3) | JPG_Leading(hwset->nleading); 
    dwset0 |= JPG_RGB_ENABLE | JPG_ENGINE_ENABLE| JPG_MCU_Restart(hwset->ninterval);  
    dwsafemode |= JPG_DOWN_SAMPLE(hwset->down_sample_rate) | JPG_HFLITER_ENABLE(hwset->filter_used);
    if (hwset->nSafeMode > 0)
    {
        dwsafemode |= JPG_SAVING_ENABLE | JPG_OUTBUF_SIZE(hwset->nMcuwidth * hwset->nSafeMode);
    }
    HAL_WRITE_UINT32(JPG_Enable, JPG_RESET_DISABLE | NR_H_BLK(hwset->nbloknum) | 0x4);//2680
    HAL_WRITE_UINT32(JPG_DIMSET, JPG_Frame_width(hwset->nMcuwidth) |JPG_Frame_height(hwset->nMcuheight));//6004
    HAL_WRITE_UINT32(JPG_DST_RGBA, JPG_Frame_base((cyg_uint32)jpeg_output >> 11));//0x6008
    HAL_WRITE_UINT32(JPG_DST_Y, JPG_Frame_base((cyg_uint32)hwset->dst_y.base >> 11));//0x600c
    HAL_WRITE_UINT32(JPG_DST_UV, JPG_Frame_base((cyg_uint32)hwset->dst_uv.base >> 11));//0x6010
    HAL_WRITE_UINT32(JPG_FLITER_COEF, TAB_FILTER(0xa, 0x2, 0x2, 0x2));//0x6010
    HAL_WRITE_UINT32(JPG_SAFE_MODE, dwsafemode);//0x602c
    HAL_WRITE_UINT32(JPG_SET0, dwset0);//0x6000
    
    return 1;
}

#ifdef CYG_HAL_CHIP_VERSION_910A
LOCAL(int) __sis2klimitation(j_decompress_ptr cinfo, display_info* display_buffer)
{   
    //The limitation of the width has 2K pixel limit (1984 pixels)
    j_hw_ptr hwset = cinfo->phwset;
    int slice_count = 0, remain_length = 0;
    cyg_uint32 dwset0 = 0, dwsafemode = 0, a = 0;

    // image
    int obj_w = cinfo->output_width;
    int obj_h = cinfo->output_height;
    int slice_length = 1024;  
    // one time 1k pixels because the jpeg engine output addr must 4K alignment
    // 1024 pixel * 4 (BYTEPERPIXEL) = 4096

    // diag_printf("obj_w : %d | obj_h : %d\n", obj_w, obj_h);
    int block_size = hwset->nblock_width;
    // diag_printf("hwset->nblock_width : %d\n", hwset->nblock_width);
    
    slice_count = obj_w / slice_length;
    remain_length = obj_w - slice_count * slice_length;

    int status_count = 0, buffersize_a = 0, buffersize_b = 0, width_pitch_a = 0; 
    int width_pitch_b = 0;
    // diag_printf("slice_count : %d | remain_length : %d\n", slice_count, remain_length);
    
    int nheight = hwset->nSafeMode * hwset->nblock_height;
    // diag_printf("nheight : %d\n", nheight);

    // temp buffer
    int width_alignment = (((display_buffer->width << 2) + 1024) & ~1023) >> 2;
    cyg_uint8 *temp_buffer = NULL;
    if((obj_h & 0xf) != 0)
        obj_h = (obj_h + 0xf) & ~(0xf);
    cyg_uint32 allocate_size = BYTEPERPIXEL * obj_h * width_alignment ;
    temp_buffer = jpeg_malloc(allocate_size, 1024, 1);
    cyg_uint32 dis_temp_buffer = (cyg_uint32)temp_buffer;
    cyg_uint32 temp_buffer_offset = nheight * width_alignment * BYTEPERPIXEL;

    // jpeg output buffer
    cyg_uint8 *jpeg_output_buffer = NULL;
    cyg_uint32 jpeg_output_buffer_p = slice_length * (slice_count + 1);
    cyg_uint32 jpeg_output_buffer_s = nheight * jpeg_output_buffer_p * BYTEPERPIXEL;
    jpeg_output_buffer = jpeg_malloc(jpeg_output_buffer_s, 4096, 1);
    cyg_uint32 jpeg_addr = (cyg_uint32)jpeg_output_buffer;

    

    // scaling
    int v_m = 0, v_n = 0, h_m = 0, h_n = 0;
    float ctemp = 0;

    if (obj_h == display_buffer->height)
    {
        v_m = v_n = 0;
    }
    else if (obj_h > display_buffer->height)
    {
        v_m = 256;
        ctemp = display_buffer->height;
        v_n = (int)((ctemp / obj_h) * 256); 
    }
    else
    {
        v_n = 256;
        ctemp = obj_h;
        v_m = (int)((ctemp / display_buffer->height) * 256); 
    }

    if (obj_w == display_buffer->width)
    {
        h_m = h_n = 0;
    }
    else if (obj_w > display_buffer->width)
    {
        h_m = 256;
        ctemp = display_buffer->width;
        h_n = (int)((ctemp / obj_w) * 256); 
    }
    else
    {
        h_n = 256;
        ctemp = obj_w;
        h_m = (int)((ctemp / display_buffer->width) * 256);
    }

    // slice count
    buffersize_a = (slice_length / block_size) * hwset->nSafeMode;

    buffersize_b = ((remain_length + block_size - 1) / block_size) * hwset->nSafeMode;
    width_pitch_a = ((buffersize_a & 0x3) != 0) ? 
                    (buffersize_a) : (buffersize_a + 2);
    width_pitch_b = ((buffersize_b & 0x3) != 0) ? 
                    (buffersize_b) : (buffersize_b + 2);

    if (slice_count == 0)
    {
        width_pitch_a = width_pitch_b;
        buffersize_a = buffersize_b ;   
    }
    R16_CACHE_FLUSH(jpeg_output_buffer, jpeg_output_buffer_s);
    // jpeg hardware setting
    dwset0 |= JPG_DMA_8X_ENABLE((hwset->bdma_8x) ? (0x1) : (0x0)) | hwformat[hwset->eSampleFmt];
    dwset0 |= JPG_DMA_RQ_LVL(3) | JPG_Leading(hwset->nleading); 
    dwset0 |= JPG_RGB_ENABLE | JPG_ENGINE_ENABLE | JPG_MCU_Restart(hwset->ninterval);      
    dwsafemode |= JPG_SAVING_ENABLE | JPG_OUTBUF_SIZE(buffersize_a);
    HAL_WRITE_UINT32(JPG_Enable, JPG_RESET_DISABLE | NR_H_BLK(width_pitch_a) | 0x4);
    HAL_WRITE_UINT32(JPG_DIMSET, JPG_Frame_width(hwset->nMcuwidth) | 
                                 JPG_Frame_height(hwset->nMcuheight));
    HAL_WRITE_UINT32(JPG_DST_RGBA, JPG_Frame_base(jpeg_addr >> 11));
    HAL_WRITE_UINT32(JPG_SAFE_MODE, dwsafemode);//0x602c  x
    HAL_WRITE_UINT32(JPG_SET0, dwset0);//0x6000   x
    // jpeg hwrdware

    int countt = 0;
    do
    {
        HAL_READ_UINT32(JPG_SAFE_MODE,a);
        if (a & JPG_OUTPUT_FULL)
        {
            if (status_count == slice_count) 
            {
                // diag_printf("countt = %d\n", countt);
                // reset buffer addr and count
                g2d_memscl(1, dis_temp_buffer,(cyg_uint32)jpeg_output_buffer, 0, 0, 0,
                    0, 0, 0, 0, 0, h_m, h_n, obj_w, nheight, jpeg_output_buffer_p,
                    width_alignment);
                dis_temp_buffer += temp_buffer_offset;
                jpeg_addr = (cyg_uint32)jpeg_output_buffer;
                status_count = 0;
                countt++;
            }
            else
            {
                // to set the outbuf addr
                jpeg_addr += slice_length * BYTEPERPIXEL * 16;
                (status_count == slice_count - 1) ? 
                (jpeg_reset(jpeg_addr,buffersize_b,width_pitch_b)):
                (jpeg_reset(jpeg_addr,buffersize_a,width_pitch_a));
                status_count++;
            }
        }
        HAL_READ_UINT32(JPG_STATUS0, a);         
    } while (!( a & JPG_HW_DONE));

    g2d_memscl(0, (cyg_uint32)display_buffer->addr, (cyg_uint32)temp_buffer, 0,
        0, 0, 0, 0, obj_h, display_buffer->height, 0, 0, 0,
        display_buffer->width, obj_h, width_alignment,  display_buffer->pitch);

    HAL_WRITE_UINT32(JPG_Enable, JPG_RESET_ENABLE | 0x4);
    jpeg_free(jpeg_output_buffer);
    jpeg_free(temp_buffer);
    return 1;
}
#endif // ~CYGPKG_JPEG_VERSION_910A

LOCAL(int) __sisdumpjpegdata(j_decompress_ptr cinfo, display_info* display_buffer)
{
    // This function is JPEG non-saving mode.
    int v_m = 0, v_n = 0, h_m = 0, h_n = 0, src_pitch = 0, a = 0;
    int obj_w = cinfo->output_width;
    int obj_h = cinfo->output_height;
    assert(obj_h < 512);
    float ctemp = 0;
    j_hw_ptr hwset = cinfo->phwset;
    int ret = 0;

    // jpeg_output_buffer
    cyg_uint32 allocsize = hwset->nbloknum * 16 * BYTEPERPIXEL * 
                           hwset->nMcuheight * 16;
    cyg_uint8 *jpeg_output = jpeg_malloc(allocsize, 4096, 1); 
    memset(jpeg_output, 255, allocsize);

    // HW JPEG setting
    R16_CACHE_FLUSH(jpeg_output, allocsize);
    ret = __sissetjpegdata(cinfo, jpeg_output);

    // HW JPEG run
    do
    {
        HAL_READ_UINT32(0x90006014, a);      
    }while(!(a & JPG_HW_DONE)); 
    driver_message("Complete!!!\n");
    HAL_WRITE_UINT32(JPG_Enable, JPG_RESET_ENABLE | 0x4);
    driver_message("__sisdumpjpegdata\n");

    if(display_buffer != NULL)
    {
        // set scaling function
        if(obj_h == display_buffer->height){v_m = v_n = 0;}
        else if(obj_h > display_buffer->height)
        {
            v_m = 256;
            ctemp = display_buffer->height;
            v_n = (int)((ctemp / obj_h) * 256); 
        }
        else
        {
            v_n = 256;
            ctemp = obj_h;
            v_m = (int)((ctemp / display_buffer->height) * 256); 
        }
        if(obj_w == display_buffer->width){h_m = h_n = 0;}
        else if(obj_w > display_buffer->width)
        {
            h_m = 256;
            ctemp = display_buffer->width;
            h_n = (int)((ctemp / obj_w) * 256); 
        }
        else
        {
            h_n = 256;
            ctemp = obj_w;
            h_m = (int)((ctemp / display_buffer->width) * 256);
        }
        src_pitch = hwset->nbloknum * hwset->nblock_width ;
        R16_CACHE_FLUSH(display_buffer->addr, display_buffer->height * 
                        display_buffer->pitch * BYTEPERPIXEL);
        // move data from jpeg output buffer to display buffer    
        if(obj_h > display_buffer->height || obj_w > display_buffer->width)
        {
            g2d_memscl(1, (cyg_uint32)display_buffer->addr, 
                (cyg_uint32)jpeg_output, 0, 0, 0, 0, 0, v_m, v_n, 0, h_m, 
                h_n, obj_w, obj_h, src_pitch, display_buffer->pitch);
        }
        else // small image
        {
            g2d_memscl(1, (cyg_uint32)display_buffer->addr, 
                (cyg_uint32)jpeg_output, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                obj_w, obj_h, src_pitch, display_buffer->pitch);
        }     
    }
    // free memory
    free(jpeg_output);
    return 1;
}

#ifdef CYG_HAL_CHIP_VERSION_910B
LOCAL(int) __sisdownsamplejpegdata(j_decompress_ptr cinfo, display_info* display_buffer)
{
    // 1. this rountine is for SIS910B, add the prescaling and filter for JPEG. 
    // 2. jpeg engine -> jpeg_output_buffer -> temp_buffer -> display buffer.
    // 3. jpeg_output_buffer address needs 4k alignment. (for JPEG)
    // 4. jpeg_output_buffer height needs 16 byte alignment. (for JPEG)
    // 5. temp_buffer and display address need 1k alignment. (for 2D scaling)
    // 6. prescaling = 1 -> h = 1/2 h
    //    prescaling = 2 -> h = 1/2 h ; w = 1/2 w 
    //    prescaling = 3 -> h = 1/2 h ; w = 1/4 w
    
    cyg_uint32 total_buffer_size = 0;
    // hardware setting 
    j_hw_ptr hwset = cinfo->phwset;
    int a = 0; 
    // display
    int output_w = display_buffer->width;
    int output_h = display_buffer->height;
    // jpeg output
    int jpeg_output_w = hwset->nMcuwidth * hwset->nblock_width; 
    int jpeg_output_p = hwset->nbloknum  * hwset->nblock_width;
    int jpeg_output_h = hwset->nSafeMode * hwset->nblock_height;    
    // temp output
    int temp_buffer_h = hwset->nMcuheight * hwset->nblock_height;
    int temp_buffer_w = output_w;
    int temp_sca_factor = 0;
    int temp_buffer_p = 0;
    int temp_buffer_offset = 0;
    
    // for 2D scaling
    float ctemp = 0;
    int v_m = 0, v_n = 0, h_m = 0, h_n = 0; 

#ifdef CYGPKG_JPEG_PRE_SCALING
    // image
    int obj_w = cinfo->output_width;
    int obj_h = cinfo->output_height;
    // The down sample rate suits with this image.
    if(((obj_h >> 2) >= output_h) && ((obj_w >> 2) >= output_w))
    // (down sample = 2) + (filter = true) --> h = 1/4 w = 1/4 
    {
        jpeg_output_h = (jpeg_output_h + 1) >> 1;
        jpeg_output_h = (jpeg_output_h + 1) >> 1;
        temp_buffer_h = (temp_buffer_h + 1) >> 1;
        temp_buffer_h = (temp_buffer_h + 1) >> 1;
        jpeg_output_w = (jpeg_output_w + 1) >> 1;
        jpeg_output_w = (jpeg_output_w + 1) >> 1;
        hwset->down_sample_rate = 2;
        hwset->filter_used = true;
    }
    else if((obj_h >> 1) >= output_h) 
    // down sample mode enable ? 
    {
        jpeg_output_h = (jpeg_output_h + 1) >> 1;
        temp_buffer_h = (temp_buffer_h + 1) >> 1;
        if((obj_w >> 2) >= output_w) 
        // down sample = 3 --> h = 1/2 w = 1/4 
        {
            hwset->down_sample_rate = 3;
            jpeg_output_w = (jpeg_output_w + 1) >> 1;
            jpeg_output_w = (jpeg_output_w + 1) >> 1;
        }
        else if((obj_w >> 1) >= output_w)
        // down sample = 2 --> h = 1/2 w = 1/2 
        {
            hwset->down_sample_rate = 2;
            jpeg_output_w = (jpeg_output_w + 1) >> 1;
        }
        else
        // down sample = 1 --> h = 1/2
        {
            hwset->down_sample_rate = 1;
        }
    }
#endif // ~CYGPKG_JPEG_PRE_SCALING
    // jpeg output buffer - need 4K alignment
    cyg_uint32 jpeg_allocsize = hwset->nbloknum * hwset->nblock_width * 
                                BYTEPERPIXEL * 16 * hwset->nSafeMode;
    cyg_uint8 *jpeg_output_buffer = jpeg_malloc(jpeg_allocsize, 4096, 1);
    #if 0
    (hwset->filter_used == false) ?
    (diag_printf("jpeg down sample rate : %d\n", hwset->down_sample_rate)) : 
    (diag_printf("jpeg down sample rate : %d\n", hwset->down_sample_rate + 2));
    diag_printf("jpeg output buffer addr : 0x%08x | size : %d\n", 
                jpeg_output_buffer, jpeg_allocsize);
    diag_printf("jpeg output buffer width : %d | pitch : %d | height : %d\n", 
                jpeg_output_w, jpeg_output_p, jpeg_output_h);
    #endif
    // Hardware JPEG setting

    R16_CACHE_FLUSH(jpeg_output_buffer, jpeg_allocsize);
    __sissetjpegdata(cinfo, jpeg_output_buffer);

    // temp buffer
    cyg_uint8 *temp_buffer = NULL;

    // compute the value well suit to the 1K alignment for scaling addr.  
    temp_sca_factor = 1024 / (jpeg_output_h * BYTEPERPIXEL);
    temp_buffer_p = (temp_buffer_w % temp_sca_factor == 0) ? 
                    (temp_buffer_w) : 
                    ((temp_buffer_w + temp_sca_factor) & (~(temp_sca_factor - 1)));
#ifdef CYGPKG_JPEG_TEMP_BUFFER_HEIGHT
    // cut temp buffer
    cyg_uint16 temp_buffer_cut_height = CYGPKG_JPEG_TEMP_BUFFER_HEIGHT;
    cyg_uint16 temp_buffer_move_cut_count = temp_buffer_cut_height / jpeg_output_h;
    cyg_uint16 jpeg_output_move_count = 0;
    cyg_uint16 display_y = 0;
    cyg_uint32 allocate_size = BYTEPERPIXEL * temp_buffer_cut_height * temp_buffer_p;

#else
    cyg_uint32 allocate_size = BYTEPERPIXEL * temp_buffer_h * temp_buffer_p;

#endif // ~CYGPKG_JPEG_TEMP_BUFFER_HEIGHT
    
    temp_buffer = malloc_align(allocate_size, 1024);
  
    if(temp_buffer == NULL)
    {
        diag_printf("temp output buffer malloc fail\n");    
    }
    #if 0
    diag_printf("temp_buffer addr : 0x%08x | size : %d\n", 
                temp_buffer, allocate_size);
    diag_printf("temp buffer width : %d | pitch : %d | height : %d\n", 
                temp_buffer_w, temp_buffer_p, temp_buffer_h);
    #endif
    // calculate the total memory size that JPEG driver need
    total_buffer_size = jpeg_allocsize + allocate_size + 
                        hwset->hopper.nsize + hwset->qmatrix.nsize +
                        hwset->srcbitstream.nsize;
    // clear memory buffer
    g2d_memset(temp_buffer, 0, temp_buffer_p, temp_buffer_h, temp_buffer_p);
    
    cyg_uint32 dis_temp_buffer = (cyg_uint32)temp_buffer;   

    // compute the scaling coefficient
    if(temp_buffer_h == output_h){v_m = v_n = 0;}
    else if(temp_buffer_h > output_h)
    {
        v_m = 256;
        ctemp = output_h;
        v_n = (int)((ctemp / temp_buffer_h) * 256); 
    }
    else
    {
        v_n = 256;
        ctemp = temp_buffer_h;
        v_m = (int)((ctemp / output_h) * 256); 
    }
    if(jpeg_output_w == temp_buffer_w){h_m = h_n = 0;}
    else if(jpeg_output_w > temp_buffer_w)
    {
        h_m = 256;
        ctemp = temp_buffer_w;
        h_n = (int)((ctemp / jpeg_output_w) * 256); 
    }
    else
    {
        h_n = 256;
        ctemp = jpeg_output_w;
        h_m = (int)((ctemp / temp_buffer_w) * 256);
    }
    //diag_printf("vn = %d | vm = %d | hn = %d | hm = %d\n", 
    //            v_n, v_m, h_n, h_m);
    
    // JPEG engine start work
    temp_buffer_offset = temp_buffer_p * jpeg_output_h * BYTEPERPIXEL;
    //diag_printf("temp_buffer_offset : %d\n", temp_buffer_offset);

    R16_CACHE_FLUSH(temp_buffer, allocate_size);
    do
    {
        HAL_READ_UINT32(JPG_SAFE_MODE,a);
        if (a & JPG_OUTPUT_FULL) // read the bit about buffer full
        {
            // for debug : 6014 6018 601c 6028 602c
            // diag_printf("dis_temp_buffer : %08x\n", dis_temp_buffer);
            g2d_memscl(1, dis_temp_buffer, (cyg_uint32)jpeg_output_buffer, 0, 0,
                    0, 0, 0, 0, 0, 0, h_m, h_n, jpeg_output_w, jpeg_output_h, 
                    jpeg_output_p, temp_buffer_p);        
            dis_temp_buffer += temp_buffer_offset;      
            
#ifdef CYGPKG_JPEG_TEMP_BUFFER_HEIGHT
            jpeg_output_move_count++;
            if(jpeg_output_move_count == temp_buffer_move_cut_count)
            {
                g2d_memscl(0, (cyg_uint32)display_buffer->addr, (cyg_uint32)temp_buffer, 0,
                        0, 0, display_y, 0, v_m, v_n, 0, 0, 0, temp_buffer_w, temp_buffer_cut_height, 
                        temp_buffer_p, display_buffer->pitch);
                jpeg_output_move_count = 0; // jpeg count reset 
                dis_temp_buffer = (cyg_uint32)temp_buffer; // temp addr reset
                if(v_n > v_m)
                {
                    display_y += temp_buffer_cut_height * v_m / v_n;
                }
                else if(v_n < v_m)
                {
                    display_y += temp_buffer_cut_height * v_n / v_m;
                }
                else
                {
                    display_y += temp_buffer_cut_height;
                }
            }
#endif // ~CYGPKG_JPEG_TEMP_BUFFER_HEIGHT
            HAL_WRITE_UINT32(JPG_SAFE_MODE, (a | JPG_DMA_RESTART));
            HAL_WRITE_UINT32(JPG_SAFE_MODE, a); 
        }
        HAL_READ_UINT32(JPG_STATUS0, a);  
    } while (!( a & JPG_HW_DONE));

    R16_CACHE_FLUSH(display_buffer->addr, display_buffer->pitch * 
                    display_buffer->height * BYTEPERPIXEL);
    
#ifdef CYGPKG_JPEG_TEMP_BUFFER_HEIGHT
    if(jpeg_output_move_count != 0)
    {
        //diag_printf("jpeg_output_move_count : %d\n", jpeg_output_move_count);
        g2d_memscl(0, (cyg_uint32)display_buffer->addr, (cyg_uint32)temp_buffer, 0,
            0, 0, display_y, 0, v_m, v_n, 0, 0, 0, temp_buffer_w, 
            jpeg_output_move_count * jpeg_output_h, temp_buffer_p, display_buffer->pitch);
    }
#else
    g2d_memscl(0, (cyg_uint32)display_buffer->addr, (cyg_uint32)temp_buffer, 0,
        0, 0, 0, 0, v_m, v_n, 0, 0, 0, temp_buffer_w, temp_buffer_h, 
        temp_buffer_p, display_buffer->pitch);    
#endif
    HAL_WRITE_UINT32(JPG_Enable, (JPG_RESET_ENABLE | 0x4));

    //diag_printf("Total memory used : %d\n", total_buffer_size);

    //driver_message("HW JPEG Complete!!!\n");
    
    // free memory
    jpeg_free(jpeg_output_buffer);
    jpeg_free(temp_buffer);

    return 1;
}

#endif

GLOBAL(int) __sishwdecode_jpg(j_decompress_ptr cinfo, display_info* display_buffer)
{
    //time_set();
    int ret = 0;
    j_hw_ptr hwset = cinfo->phwset;
    driver_message("__sishwdecode_jpg\n");   
    //step1. fill hwsetting, including bitstream, hopper and qmatrix
    ret = __sisfilljpegdata(cinfo, display_buffer);
    if (ret!=1)
    {
        driver_message("__sisfilljpegdata error!!!\n");
    }
    //step2. load to hw DMA, include bitstream, hopper and qmatrix
    ret = __sisloadjpegdata(cinfo);
    if (ret!=1)
    {
        driver_message("__sisloadjpegdata error!!!\n");
    }
    //step3. other hw setting and JPEG execute

    if(hwset->nSafeMode > 0) 
    // saving mode
    {
#ifdef CYG_HAL_CHIP_VERSION_910B
        ret = __sisdownsamplejpegdata(cinfo, display_buffer);
#elif defined(CYG_HAL_CHIP_VERSION_910A)
        ret = __sis2klimitation(cinfo, display_buffer);
#endif
    }
    else 
    // non-saving mode
    {
        ret = __sisdumpjpegdata(cinfo, display_buffer);
    }

    //time_get();
    return ret;
}

void JPEG_read(void)
{
    diag_printf("JPEG Register Table\n");
    diag_printf("0x90002680 : %08x\n", HAL_Value_Read_UINT32(0x90002680));
    diag_printf("0x90006000 : %08x\n", HAL_Value_Read_UINT32(0x90006000));
    diag_printf("0x90006004 : %08x\n", HAL_Value_Read_UINT32(0x90006004));
    diag_printf("0x90006008 : %08x\n", HAL_Value_Read_UINT32(0x90006008));
    diag_printf("0x9000600c : %08x\n", HAL_Value_Read_UINT32(0x9000600c));
    diag_printf("0x90006010 : %08x\n", HAL_Value_Read_UINT32(0x90006010));
    diag_printf("0x90006014 : %08x\n", HAL_Value_Read_UINT32(0x90006014));
    diag_printf("0x90006018 : %08x\n", HAL_Value_Read_UINT32(0x90006018));
    diag_printf("0x9000601c : %08x\n", HAL_Value_Read_UINT32(0x9000601c));
    diag_printf("0x90006020 : %08x\n", HAL_Value_Read_UINT32(0x90006020));
    diag_printf("0x90006024 : %08x\n", HAL_Value_Read_UINT32(0x90006024));
    diag_printf("0x90006028 : %08x\n", HAL_Value_Read_UINT32(0x90006028));
    diag_printf("0x9000602c : %08x\n", HAL_Value_Read_UINT32(0x9000602c));
    diag_printf("0x90006030 : %08x\n", HAL_Value_Read_UINT32(0x90006030));
    diag_printf("0x90006034 : %08x\n", HAL_Value_Read_UINT32(0x90006034));
    diag_printf("0x90006038 : %08x\n", HAL_Value_Read_UINT32(0x90006038));
}

void jpeg_reset(cyg_uint32 addr, cyg_uint32 buffersize, cyg_uint32 pitch)
{
    cyg_uint32 dwsafemode = 0;
    HAL_READ_UINT32(JPG_SAFE_MODE, dwsafemode);
    dwsafemode &= ~(JPG_OUTBUF_SIZE(0xFFFF));
    dwsafemode |= JPG_OUTBUF_SIZE(buffersize);
    HAL_WRITE_UINT32(JPG_Enable, JPG_RESET_DISABLE | NR_H_BLK(pitch) | 0x4);
    HAL_WRITE_UINT32(JPG_DST_RGBA, JPG_Frame_base(addr >> 11));
    HAL_WRITE_UINT32(JPG_SAFE_MODE, (dwsafemode | JPG_DMA_RESTART));
    HAL_WRITE_UINT32(JPG_SAFE_MODE, dwsafemode);
}

