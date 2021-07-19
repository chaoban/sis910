#include <stdio.h>
#include <stdlib.h>
#include <cyg/io/io.h>
#include <cyg/io/display/g2d/G2D_io.h>

cyg_uint8 dmac_sel = 0;

typedef struct{
    cyg_uint32 addr;
    cyg_uint8 choose;
	bool on;
}G2D_flip_data;

typedef struct{
	ALPBLN_MODE mode;
	bool updatedstalpha;
	bool dst_en_def_alpha;
	bool src_en_def_alpha;
	cyg_uint8 dst_def_alpha;
	cyg_uint8 src_def_alpha;
}G2D_alpha_data;

typedef struct{
	cyg_uint32 dst_frmb;
	cyg_uint32 src_frmb;
	cyg_uint32 src_x;
	cyg_uint32 src_y;
	cyg_uint32 dst_x;
	cyg_uint32 dst_y;
    cyg_uint32 vsr_i;
	cyg_uint32 vsr_n;
	cyg_uint32 vsr_m;
    cyg_uint32 hsr_i;
	cyg_uint32 hsr_n;
	cyg_uint32 hsr_m;
    bool       flag;
//T-scaling    
    bool       t_scale;
    cyg_uint8  t_lu_point;
    cyg_uint8  t_line_point;
    cyg_uint8  t_direction;  //1 : hor 2: ver
    cyg_uint8  t_width;
//wrap-mode
    bool       wrap_mode;
    cyg_uint16 wrap_low_height;
    cyg_uint16 wrap_high_height;
    bool       wrap_low_ready;
    bool       wrap_high_ready;
    cyg_uint16 wrap_count;
    cyg_uint16 wrap_total;
}G2D_scaling_data;

typedef struct{
	cyg_uint8 opr_cmd;
	cyg_uint8 choice;
	cyg_uint8 *dst;
	cyg_uint8 *src;
	cyg_uint8 dmac_sel;
	int obj_width;
	int obj_height;
	int src_width; 
	int dst_width;
	int ropf;
	int pattern;
	int rate;
	int rotat;
  	cyg_uint32 *pal_table;
	cyg_uint8 pal_data;
	G2D_alpha_data alpha_data;
	G2D_scaling_data scaling_data;
    G2D_flip_data flip_data;
    bool comdq_empty;
}G2D_data;

G2D_data sis_g2d = {0};
cyg_uint32 g2d_len = sizeof(sis_g2d);

cyg_io_handle_t sisg2d_handle = NULL;
Cyg_ErrNo g2d_err;

int g2d_dev_test(void)
{
	int re = 0;
	g2d_err = cyg_io_lookup("/dev/display/g2d", &sisg2d_handle);
	if(ENOERR != g2d_err || NULL == sisg2d_handle) 
    {
  	    diag_printf("G2D IO cyg_io_lookup error!!! \n");
        re = -1;
    }
	return re;
}
int g2d_fun_test(int err)
{
	int re = 0;
    if(ENOERR != err) 
    {
        diag_printf("G2D IO cyg_io_set_config error!! \n");
	    re = -1;
    }
    return re;
}

int g2d_memcpy(cyg_uint8 *dst, cyg_uint8 *src, int obj_width, int obj_height, int src_width, int dst_width)
{
	sis_g2d.dst = dst;
	sis_g2d.src = src;
	sis_g2d.dmac_sel = dmac_sel;
	sis_g2d.obj_width = obj_width;
	sis_g2d.obj_height = obj_height;
	sis_g2d.src_width = src_width;
	sis_g2d.dst_width = dst_width;
	sis_g2d.rate = 1;
	
	g2d_dev_test();
	g2d_err = cyg_io_set_config(sisg2d_handle, 5, &sis_g2d, &g2d_len);
	g2d_fun_test(g2d_err);
	return 0;
}
int g2d_memset(cyg_uint8 *dst, int val, int obj_width, int obj_height, int dst_width)
{
	sis_g2d.dst = dst; 
	sis_g2d.dmac_sel = dmac_sel;
	sis_g2d.obj_width = obj_width;
	sis_g2d.obj_height = obj_height; 
	sis_g2d.dst_width = dst_width;
	sis_g2d.pattern = val;
	sis_g2d.rate = 1;
	
	g2d_dev_test();
	g2d_err=cyg_io_set_config(sisg2d_handle, 3, &sis_g2d, &g2d_len);
	g2d_fun_test(g2d_err);
	return 0;
}
void g2d_alpha_blend(cyg_uint8 *dst, cyg_uint8 *src, ALPBLN_MODE mode, int obj_width,
                     int obj_height, int src_width, int dst_width, bool f_dst_ab,
                     bool dst_ini_ab, cyg_uint8 dst_def_alpha, bool src_ini_ab, cyg_uint8 src_def_alpha)
{
	sis_g2d.dst = dst;
	sis_g2d.src = src;
	sis_g2d.dmac_sel = dmac_sel;
	sis_g2d.obj_width = obj_width;
	sis_g2d.obj_height = obj_height;
	sis_g2d.src_width = src_width;
	sis_g2d.dst_width = dst_width;
	sis_g2d.alpha_data.dst_def_alpha = dst_def_alpha;
	sis_g2d.alpha_data.dst_en_def_alpha = dst_ini_ab;
	sis_g2d.alpha_data.mode = mode;
	sis_g2d.alpha_data.src_def_alpha = src_def_alpha;
	sis_g2d.alpha_data.src_en_def_alpha = src_ini_ab;
	sis_g2d.alpha_data.updatedstalpha = f_dst_ab;
	sis_g2d.rate = 1;

	g2d_dev_test();
	g2d_err=cyg_io_set_config(sisg2d_handle, 2, &sis_g2d, &g2d_len);
	g2d_fun_test(g2d_err);
}
int g2d_memmix(cyg_uint8 *dst, cyg_uint8 *src, int ropf, int pattern, int obj_width,
               int obj_height, int src_width, int dst_width)
{
	sis_g2d.dst = dst;
	sis_g2d.src = src;
	sis_g2d.dmac_sel = dmac_sel;
	sis_g2d.ropf = ropf;
	sis_g2d.pattern = pattern;
	sis_g2d.obj_width = obj_width;
	sis_g2d.obj_height = obj_height;
	sis_g2d.src_width = src_width;
	sis_g2d.dst_width = dst_width;
	sis_g2d.rate = 1;

	g2d_dev_test();
	g2d_err=cyg_io_set_config(sisg2d_handle, 6, &sis_g2d, &g2d_len);
	g2d_fun_test(g2d_err);
	return 0;
}

//flag = 1 --> for Jpeg mode
//flag = 0 --> for G2D mode
int g2d_memscl(bool flag, cyg_uint32 dst_frmb, cyg_uint32 src_frmb,
               cyg_uint32 src_x, cyg_uint32 src_y, cyg_uint32 dst_x, cyg_uint32 dst_y,
               cyg_uint32 vsr_i, cyg_uint32 vsr_n, cyg_uint32 vsr_m,
               cyg_uint32 hsr_i, cyg_uint32 hsr_n, cyg_uint32 hsr_m,
               int obj_width, int obj_height, int src_width, int dst_width)
{
    sis_g2d.scaling_data.flag = flag;
	sis_g2d.scaling_data.dst_frmb = dst_frmb;
	sis_g2d.scaling_data.src_frmb = src_frmb;
	sis_g2d.dmac_sel = dmac_sel;
	sis_g2d.scaling_data.src_x = src_x;
	sis_g2d.scaling_data.src_y = src_y;
	sis_g2d.scaling_data.dst_x = dst_x;
	sis_g2d.scaling_data.dst_y = dst_y;
	sis_g2d.scaling_data.vsr_i = vsr_i;
	sis_g2d.scaling_data.vsr_m = vsr_m;
	sis_g2d.scaling_data.vsr_n = vsr_n;
	sis_g2d.scaling_data.hsr_i = hsr_i;
	sis_g2d.scaling_data.hsr_m = hsr_m;
	sis_g2d.scaling_data.hsr_n = hsr_n;
    sis_g2d.scaling_data.t_direction = 0;
    sis_g2d.scaling_data.t_line_point = 0;
    sis_g2d.scaling_data.t_lu_point = 0;
    sis_g2d.scaling_data.t_scale = 0;
    sis_g2d.scaling_data.t_width = 0;
    sis_g2d.obj_width = obj_width;
	sis_g2d.obj_height = obj_height;
	sis_g2d.src_width = src_width;
	sis_g2d.dst_width = dst_width;
    sis_g2d.scaling_data.wrap_mode = false;
	sis_g2d.scaling_data.t_scale= false;
    sis_g2d.scaling_data.wrap_high_height = 0;
    sis_g2d.scaling_data.wrap_low_height = 0;
    sis_g2d.scaling_data.wrap_high_ready = false;
    sis_g2d.scaling_data.wrap_low_ready = false;
	g2d_dev_test();
	g2d_err=cyg_io_set_config(sisg2d_handle, 7, &sis_g2d, &g2d_len);
	g2d_fun_test(g2d_err);
	return 0;
}

#ifdef CYG_HAL_CHIP_VERSION_910B
int g2d_wrap(bool flag, cyg_uint32 dst_frmb, cyg_uint32 src_frmb, cyg_uint32 src_x, 
               cyg_uint32 src_y, cyg_uint32 dst_x, cyg_uint32 dst_y,
               cyg_uint32 vsr_i, cyg_uint32 vsr_n, cyg_uint32 vsr_m,
               cyg_uint32 hsr_i, cyg_uint32 hsr_n, cyg_uint32 hsr_m,
               int obj_width, int obj_height, int src_width, int dst_width,
               cyg_uint16 low_h, cyg_uint16 high_h)
{
    sis_g2d.scaling_data.flag = flag;
	sis_g2d.scaling_data.dst_frmb = dst_frmb;
	sis_g2d.scaling_data.src_frmb = src_frmb;
	sis_g2d.dmac_sel = dmac_sel;
	sis_g2d.scaling_data.src_x = src_x;
	sis_g2d.scaling_data.src_y = src_y;
	sis_g2d.scaling_data.dst_x = dst_x;
	sis_g2d.scaling_data.dst_y = dst_y;
	sis_g2d.scaling_data.vsr_i = vsr_i;
	sis_g2d.scaling_data.vsr_m = vsr_m;
	sis_g2d.scaling_data.vsr_n = vsr_n;
	sis_g2d.scaling_data.hsr_i = hsr_i;
	sis_g2d.scaling_data.hsr_m = hsr_m;
	sis_g2d.scaling_data.hsr_n = hsr_n;
    sis_g2d.scaling_data.t_direction = 0;
    sis_g2d.scaling_data.t_line_point = 0;
    sis_g2d.scaling_data.t_lu_point = 0;
    sis_g2d.scaling_data.t_scale = false;
    sis_g2d.scaling_data.t_width = 0;
    sis_g2d.scaling_data.wrap_mode = true;
    sis_g2d.scaling_data.wrap_high_height = high_h;
    sis_g2d.scaling_data.wrap_low_height = low_h;
    sis_g2d.scaling_data.wrap_high_ready = false;
    sis_g2d.scaling_data.wrap_low_ready = false;
    sis_g2d.scaling_data.wrap_count = 0;
    sis_g2d.obj_width = obj_width;
	sis_g2d.obj_height = obj_height;
	sis_g2d.src_width = src_width;
	sis_g2d.dst_width = dst_width;
    sis_g2d.scaling_data.wrap_total = (obj_height % low_h) ? 
                                      ((obj_height / low_h) + 1):((obj_height / low_h));
	
	g2d_dev_test();
	g2d_err=cyg_io_set_config(sisg2d_handle, 7, &sis_g2d, &g2d_len);
	g2d_fun_test(g2d_err);
	return 0;
}

int g2d_wrap_flip(bool flag)
{
    if(flag)// high
    {
        sis_g2d.scaling_data.wrap_low_ready = false;
        sis_g2d.scaling_data.wrap_high_ready = true;

    }
    else    // low
    {
        sis_g2d.scaling_data.wrap_low_ready = true;
        sis_g2d.scaling_data.wrap_high_ready = false;
    }
	g2d_dev_test();
	g2d_err=cyg_io_set_config(sisg2d_handle, 1, &sis_g2d, &g2d_len);
	g2d_fun_test(g2d_err);
    return 0;
}

int g2d_t_scaling(bool flag, cyg_uint32 dst_frmb, cyg_uint32 src_frmb, 
               cyg_uint32 src_x, cyg_uint32 src_y, cyg_uint32 dst_x, cyg_uint32 dst_y,
               cyg_uint32 vsr_i, cyg_uint32 vsr_n, cyg_uint32 vsr_m,
               cyg_uint32 hsr_i, cyg_uint32 hsr_n, cyg_uint32 hsr_m,
               int obj_width, int obj_height, int src_width, int dst_width,
               cyg_uint8 t_direction, cyg_uint8 lu_diff, 
               cyg_uint8 line_diff, cyg_uint8 seg_width)
{
    sis_g2d.scaling_data.flag = flag;
	sis_g2d.scaling_data.dst_frmb = dst_frmb;
	sis_g2d.scaling_data.src_frmb = src_frmb;
	sis_g2d.dmac_sel = dmac_sel;
	sis_g2d.scaling_data.src_x = src_x;
	sis_g2d.scaling_data.src_y = src_y;
	sis_g2d.scaling_data.dst_x = dst_x;
	sis_g2d.scaling_data.dst_y = dst_y;
	sis_g2d.scaling_data.vsr_i = vsr_i;
	sis_g2d.scaling_data.vsr_m = vsr_m;
	sis_g2d.scaling_data.vsr_n = vsr_n;
	sis_g2d.scaling_data.hsr_i = hsr_i;
	sis_g2d.scaling_data.hsr_m = hsr_m;
	sis_g2d.scaling_data.hsr_n = hsr_n;
    sis_g2d.scaling_data.t_direction = t_direction;
    sis_g2d.scaling_data.t_line_point = line_diff;
    sis_g2d.scaling_data.t_lu_point = lu_diff;
    sis_g2d.scaling_data.t_scale = true;
    sis_g2d.scaling_data.t_width = seg_width;
    sis_g2d.scaling_data.wrap_mode = false;
    sis_g2d.scaling_data.wrap_high_height = 0;
    sis_g2d.scaling_data.wrap_low_height = 0;
    sis_g2d.scaling_data.wrap_high_ready = false;
    sis_g2d.scaling_data.wrap_low_ready = false;
    sis_g2d.obj_width = obj_width;
	sis_g2d.obj_height = obj_height;
	sis_g2d.src_width = src_width;
	sis_g2d.dst_width = dst_width;
	
	g2d_dev_test();
	g2d_err=cyg_io_set_config(sisg2d_handle, 7, &sis_g2d, &g2d_len);
	g2d_fun_test(g2d_err);
	return 0;
}
#endif

int g2d_memmap(cyg_uint8 *dst, cyg_uint8 *src, int obj_width, 
               int obj_height, int src_width, int dst_width, cyg_uint32 
               *palette_table, cyg_uint8 pal_entry)
{
	sis_g2d.dst = dst;
	sis_g2d.src = src;
	sis_g2d.dmac_sel = dmac_sel;
	sis_g2d.obj_width = obj_width;
	sis_g2d.obj_height = obj_height;
	sis_g2d.src_width = src_width;
	sis_g2d.dst_width = dst_width;
	sis_g2d.pal_table = palette_table;
	sis_g2d.pal_data = pal_entry;
	sis_g2d.rate = 2;
	
	g2d_dev_test();
	g2d_err=cyg_io_set_config(sisg2d_handle, 9, &sis_g2d, &g2d_len);
	g2d_fun_test(g2d_err);
	return 0;
}
int g2d_rotation(cyg_uint8 *dst, cyg_uint8 *src, int rate,
                 int rotat, int obj_width, int obj_height, int src_width, int dst_width)
{
	sis_g2d.dst = dst;
	sis_g2d.src = src;
	sis_g2d.dmac_sel = dmac_sel;
	sis_g2d.obj_width = obj_width;
	sis_g2d.obj_height = obj_height;
	sis_g2d.src_width = src_width;
	sis_g2d.dst_width = dst_width;
	sis_g2d.rate = rate;
	sis_g2d.rotat = rotat;
	sis_g2d.rate = 1;
	
	g2d_dev_test();
	g2d_err=cyg_io_set_config(sisg2d_handle, 8, &sis_g2d, &g2d_len);
	g2d_fun_test(g2d_err);
	return 0;
}

void g2d_bilinear(cyg_uint8 *src, int s_w, int s_h, cyg_uint8 *dst, float h_sca, float v_sca)
{
	float B_a = 0, B_b = 0, a = 0, b = 0, c = 0, d = 0; //alpha beta
    int i = 0, j = 0, k = 0, d_h = 0, d_w = 0, s_x = 0, s_y = 0;
    cyg_uint32 total = 0, total_1 = 0;
    d_w = (int)(s_w * h_sca);
    d_h = (int)(s_h * v_sca);
	for(j = 0; j < d_h; j++)
    {
  	    for(i = 0; i < d_w; i++)
        {		
    	    s_x = (int)(i / h_sca);
			s_y = (int)(j / v_sca);
		    if(s_x >= (s_w - 1)){s_x = (s_w - 2);}
		    if(s_y >= (s_h - 1)){s_y = (s_h - 2);}
			B_a = (i / h_sca) - s_x;
			B_b = (j / v_sca) - s_y;
									
			a = (1 - B_a) * (1 - B_b);
			b = B_a * (1 - B_b);
			c = (1 - B_a) * B_b;
			d = B_a * B_b;
											
			total = (s_y * s_w + s_x) << 2;
			total_1 = (j * d_w + i) << 2;
			for(k = 0; k < 4; k++)
			{
				dst[total_1+k]=(cyg_uint8)
					(a * src[total+k] + b * src[total+4+k]+c * 
					src[total+(s_w<<2)+k] + d * src[total+(s_w<<2)+4+k]);
			}
        }     		
	 }
}

cyg_uint8* g2d_rgb2a(cyg_uint8 *src,int w,int h,cyg_uint8 a)
{
	 int i = 0, j = 0, k = 0;
	 cyg_uint8 *dst = NULL;
	 cyg_uint32 temp1 = 0, temp2 = 0;
	 int s_pitch = w * 3, d_pitch = w << 2;
	 dst = (cyg_uint8 *)malloc(sizeof(cyg_uint8) * h * d_pitch);
	 for(j = 0; j < h; j++)
     {
        for(i = 0, k = 0; i < d_pitch; i+=4, k+=3)
    	{   	
    		temp1 = j * d_pitch + i;
    		temp2 = j * s_pitch + k;
    		dst[temp1]  = (cyg_uint8)a;
    		dst[temp1+1]= (cyg_uint8)src[temp2];
    		dst[temp1+2]= (cyg_uint8)src[temp2+1];
    		dst[temp1+3]= (cyg_uint8)src[temp2+2];
    	}
     }
     return dst;
}

cyg_uint8* g2d_a2rgb(cyg_uint8 *src,int w,int h)
{
	 int i = 0, j = 0, k = 0;
	 cyg_uint8 *dst = NULL;
	 cyg_uint32 temp1 = 0, temp2 = 0;
	 int s_pitch = w << 2, d_pitch = w * 3;
	 dst = (cyg_uint8 *)malloc(sizeof(cyg_uint8) * h * d_pitch);
	 for(j = 0; j < h; j++)
     {			
	    for(i = 0, k = 0;i < d_pitch; i+=3, k+=4)
    	{   	
    		temp1 = j * d_pitch + i;
    		temp2 = j * s_pitch + k;
    		dst[temp1]   = (cyg_uint8)src[temp2+1];
    		dst[temp1+1] = (cyg_uint8)src[temp2+2];
    		dst[temp1+2] = (cyg_uint8)src[temp2+3];
    	}
     }
	 return dst;	
}

void g2d_save_bmp(char *buffer,int w,int h,int pitch,char *filename)
{
	 cyg_uint8 OffsetBits = 54;
	 cyg_uint8 buf[OffsetBits];
	 cyg_uint8 offset = 0;
	 int i = 0,j = 0;
	 char *pStart = NULL, *pNew = NULL, *buftmp = NULL;
	 unsigned int nSize = 0;
	 cyg_uint8 source_choice = 0;
	 int y = 0, x = 0;
	 if(((3 * w) % 4) != 0)
	  {offset = 4 - ((3 * w) % 4);}
	 FILE *fp1;
	   
	 cyg_uint32 InfoSize = (w + offset) * h * 3 * sizeof(cyg_uint8);
	 cyg_uint32 Size = InfoSize + OffsetBits;
	 cyg_uint8 bitperpixel = 24,plane = 1,bisize = 40;
	 for(i = 0; i < OffsetBits; i++)
	 {
		buf[i] = 0;
	 }
	   
	   buf[0] = 0x42;
	   buf[1] = 0x4D;
	   buf[2] = (Size & 0xff);
	   buf[3] = ((Size >> 8) & 0xff);
	   buf[4] = ((Size >> 16) & 0xff);
	   buf[5] = ((Size >> 24) & 0xff);
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
	   buf[15] = (bisize >> 8) & 0xff;
	   buf[14] = (bisize) & 0xff;
	   if ((fp1 = fopen(filename, "wb")) == NULL) 
	   {
		  fprintf(stderr, "can't open %s\n", filename);
       }
       else
       {
          fwrite (buf, sizeof(cyg_uint8), 54, fp1);
	   }
	   nSize = (w + offset) * h * 3;
	   pStart = pNew =(char *)malloc(nSize);
	   buftmp = (char *)buffer;
	   if(source_choice == 0) //from bmp.h 
	   {
		   for (y = h - 1; y >= 0; y--)
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
			for (y = h - 1; y >= 0; y--)
		    {
				buffer = buftmp + (y * pitch);
				for (x = 0; x < w ; x++)
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

void SW_memcpy(cyg_uint8 *dst, cyg_uint8 *src, int obj_width, int obj_height, int src_width, int dst_width)
{
    /*
    src_width = src_width << 2; 
    dst_width = dst_width << 2;
    obj_width = obj_width << 2;
    int i = 0, j = 0, k = 0;
	cyg_int32 temp = 0, temp1 = 0;

	for(j = 0; j < obj_height; j++)
	{
		for(i = 0,k = 0; i < obj_width; i += 4, k += 4)
		{
			temp  = j * src_width + i;
			temp1 = j * dst_width + k;
			dst[temp]   = src[temp1];
			dst[temp+1] = src[temp1+1];
			dst[temp+2] = src[temp1+2];
			dst[temp+3] = src[temp1+3];
		}       
	}
	*/
	cyg_uint16 i = 0, j = 0;
    cyg_uint32 src_temp = 0, dst_temp = 0;
    cyg_uint32 *src_addr = (cyg_uint32 *)src;
    cyg_uint32 *dst_addr = (cyg_uint32 *)dst;
    for(j = 0; j < obj_height; j++)
	{
		for(i = 0; i < obj_width; i++)
		{
			src_temp = j * src_width + i;
            dst_temp = j * dst_width + i;
            dst_addr[dst_temp] = src_addr[src_temp];
		}
	}
}

void SW_memset(cyg_uint8 *dst, cyg_uint32 color, int obj_width, int obj_height, int dst_width)
{
    cyg_uint32 *temp_addr = (cyg_uint32 *)dst; 
    int i = 0, j = 0;
    cyg_uint32 index_j = 0, index_i = 0; 
    for(j = 0; j < obj_height; j++)
    {
        index_j = j * dst_width;
        for(i = 0; i < obj_width; i++)
        {
            index_i = index_j + i;
            temp_addr[index_i] = color;
        }
    }
}

