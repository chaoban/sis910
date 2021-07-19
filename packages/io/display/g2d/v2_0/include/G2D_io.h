#ifndef _G2D_io_H_
#define _G2D_io_H_

#include <pkgconf/system.h>
#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>		// For diagnostic printing.
#include <cyg/io/display/crt/Crt_io.h>
typedef enum {
    ROPF_8_ZEROES        = 0x00,  /*   0 */
    ROPF_NOT_P_AND_NOT_D = 0x05,  /*   5 */
    ROPF_NOT_S_AND_NOT_D = 0x11,  /*  17 */ 
    ROPF_NOT_S_AND_D     = 0x22,  /*  34 */
    ROPF_NOT_S           = 0x33,  /*  51 */
    ROPF_S_AND_NOT_D     = 0x44,  /*  68 */
    ROPF_NOT_D           = 0x55,  /*  85 */
    ROPF_P_XOR_D         = 0x5a,  /*  90 */
    ROPF_S_XOR_D         = 0x66,  /* 102 */
    ROPF_NOT_S_OR_NOT_D  = 0x77,  /* 119 */
    ROPF_S_AND_D         = 0x88,  /* 136 */
    ROPF_D               = 0xaa,  /* 170 */
    ROPF_NOT_S_OR_D      = 0xbb,  /* 187 */
    ROPF_S_AND_P         = 0xc0,  /* 192 */
    ROPF_S               = 0xcc,  /* 204 */
    ROPF_NOT_P_OR_S      = 0xcf,  /* 207 */
    ROPF_S_OR_NOT_D      = 0xdd,  /* 221 */
    ROPF_S_OR_D          = 0xee,  /* 238 */
    ROPF_P               = 0xf0,  /* 240 */
    ROPF_NOT_S_OR_D_OR_P = 0xfb,  /* 251 */
    ROPF_8_ONES          = 0xff,  /* 255 */
    ROPF_S_XOR_NOT_D     = 0x09,  /*   9 */
} G2D_ROPF; // Raster Operating functions
/// Alpha-blending modes
typedef enum {
    ALPBLN_MODE_CLEAR = 0,
    ALPBLN_MODE_SRC,
    ALPBLN_MODE_SRC_IN,
    ALPBLN_MODE_SRC_OUT,
    ALPBLN_MODE_SRC_OVER,
    ALPBLN_MODE_DST_IN,
    ALPBLN_MODE_DST_OUT,
    ALPBLN_MODE_DST_OVER,
    ALPBLN_MODE_PRE_MUL,
    ALPBLN_MODE_ALP_EXT,
} ALPBLN_MODE;

typedef enum {
    ROTATION_DISABLE = 0,
    ROTATION_90      = 2,
    ROTATION_270     = 3,
    ROTATION_180     = 4,
    ROTATION_HOR     = 5,
    ROTATION_VER     = 6,
} ROTATION_MODE;

typedef enum {
	  ONE_ONE        = 0,
	  FOUR_FOUR      = 1,
	  ONE_FOUR       = 2,
} BPP_MODE;

/**
<para>  |<describe>
dst          The address of the dest. plane (upper left corner). Must be 1K byte aligned.
src          The address of the source plane (upper left corner). Must be 1K byte aligned.
src_x        X-coord. of the upper left corner of source object.
src_y        Y-coord. of the upper left corner of source object.
dst_x        X-coord of where to put the scaled object in dest. plane
dst_y        Y-coord of where to put the scaled object in dest. plane
rate         (0) -- source and dest planes are both 1 byte/pixel
             (1) -- source and dest planes are both 4 byte/pixel
             (2) -- source plane is 1 byte/pixel; dest plane is 4 byte/pixel
rotat        (0)disable
   	         (1)reserve
   	         (2)rotation 90 (counterclockwise)
   	         (3)rotation270
   	         (4)rotation 180
   	         (5)horizontal mirror
   	         (6)vertical mirror
   	         (7)reserve
pattern      The pattern used in the ROP function.
vsr_i        The integer part (I) of the vertical scaling ratio.
vsr_n        The numerator part (N) of the vertical scaling ratio.
vsr_m        The denominator part (M) of the vertical scaling ratio.
hsr_i        The integer part (I) of the horizontal scaling ratio.
hsr_n        The numerator part (N) of the horizontal scaling ratio.
hsr_m        The denominator part (M) of the horizontal scaling ratio.
obj_width    The width of the source rectangular object.
obj_height   The height of the source rectangular object.
src_width    The width of source plane.
dst_width    The width of dest. plane.
h_table      The initial coefficient table
v_table      The initial coefficient table
en_init_tab  1 : enable   0 : disable

addr         The start address of framebuffer
choose       The # of framebuffer : 0 or 1
on           1:  on ;  0:  off

updatedstalpha      write alpha back to destionation 1: use alpha output of alpha blending  ; 0: use original destination alpha
dst_en_def_alpha    1: use default alpha to replace destination alpha to do alpha blending
src_en_def_alpha    1: use default alpha to replace source alpha to do alpha blending; 
dst_def_alpha       destination default alpha
src_def_alpha       source default alpha

dmac_sel           DMA control
                   dmac_sel[2]-(1) out output data uses DMAC2
                               (0) out output data uses DMAC0
                   dmac_sel[1]-(1) dst input data uses DMAC2
                               (0) dst input data uses DMAC0
                   dmac_sel[0]-(1) src input data uses DMAC2
                               (0) src input data uses DMAC0

**/
/*private*/
int g2d_dev_test(void);
int g2d_fun_test(int err);

/*public*/
int g2d_memcpy(cyg_uint8 *dst, cyg_uint8 *src, int obj_width, int obj_height, int src_width, int dst_width);
int g2d_memset(cyg_uint8 *dst, int val, int obj_width, int obj_height, int dst_width);
void g2d_alpha_blend(cyg_uint8 *dst, cyg_uint8 *src, ALPBLN_MODE mode, int obj_width, 
                     int obj_height, int src_width, int dst_width, bool f_dst_ab,
                     bool dst_ini_ab, cyg_uint8 dst_def_alpha, bool src_ini_ab, cyg_uint8 src_def_alpha);
int g2d_memmix(cyg_uint8 *dst, cyg_uint8 *src, int ropf,
               int pattern, int obj_width, int obj_height, int src_width, int dst_width);
int g2d_memscl(bool flag, cyg_uint32 dst_frmb, cyg_uint32 src_frmb, 
               cyg_uint32 src_x, cyg_uint32 src_y, cyg_uint32 dst_x, cyg_uint32 dst_y,
               cyg_uint32 vsr_i, cyg_uint32 vsr_n, cyg_uint32 vsr_m,
               cyg_uint32 hsr_i, cyg_uint32 hsr_n, cyg_uint32 hsr_m,
               int obj_width, int obj_height, int src_width, int dst_width);
#ifdef CYG_HAL_CHIP_VERSION_910B
int g2d_t_scaling(bool flag, cyg_uint32 dst_frmb, cyg_uint32 src_frmb, 
               cyg_uint32 src_x, cyg_uint32 src_y, cyg_uint32 dst_x, cyg_uint32 dst_y,
               cyg_uint32 vsr_i, cyg_uint32 vsr_n, cyg_uint32 vsr_m,
               cyg_uint32 hsr_i, cyg_uint32 hsr_n, cyg_uint32 hsr_m,
               int obj_width, int obj_height, int src_width, int dst_width,
               cyg_uint8 t_direction, cyg_uint8 lu_diff, 
               cyg_uint8 line_diff, cyg_uint8 seg_width);
int g2d_wrap(bool flag, cyg_uint32 dst_frmb, cyg_uint32 src_frmb, 
               cyg_uint32 src_x, cyg_uint32 src_y, cyg_uint32 dst_x, cyg_uint32 dst_y,
               cyg_uint32 vsr_i, cyg_uint32 vsr_n, cyg_uint32 vsr_m,
               cyg_uint32 hsr_i, cyg_uint32 hsr_n, cyg_uint32 hsr_m,
               int obj_width, int obj_height, int src_width, int dst_width,
               cyg_uint16 low_h, cyg_uint16 high_h);
int g2d_wrap_flip(bool flag);
#endif //~CYG_HAL_CHIP_VERSION_910B
int g2d_memmap(cyg_uint8 *dst, cyg_uint8 *src, int obj_width, 
               int obj_height, int src_width, int dst_width,
               cyg_uint32 *palette_table, cyg_uint8 pal_entry);
int g2d_rotation(cyg_uint8 *dst, cyg_uint8 *src, int rate,   
               int rotat, int obj_width, int obj_height, int src_width, int dst_width);


void g2d_bilinear(cyg_uint8 *src, int s_w, int s_h, cyg_uint8 *dst, float h_sca, float v_sca);

void g2d_save_bmp(char *buffer, int w, int h, int pitch, char *filename);
cyg_uint8* g2d_rgb2a(cyg_uint8 *src, int w, int h, cyg_uint8 a);
cyg_uint8* g2d_a2rgb(cyg_uint8 *src, int w, int h);

void SW_memcpy(cyg_uint8 *dst, cyg_uint8 *src, int obj_width, int obj_height, int src_width, int dst_width);
void SW_memset(cyg_uint8 *dst, cyg_uint32 color, int obj_width, int obj_height, int dst_width);

#endif

