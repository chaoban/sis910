#ifndef _G2D_dev_H_
#define _G2D_dev_H_

#include <pkgconf/system.h>
#include <cyg/kernel/kapi.h> //cyg_vector
#include <errno.h>                 // Cyg_ErrNo
#include <cyg/io/devtab.h>
#include <cyg/infra/diag.h>
#include <assert.h>

/*G2D*/
/****************************hardware*****************************/
//G2D register macro
/* 0x8000 */
#define out_sel2(x)						((x & 0x1) << 24 )
#define idst_sel2(x)  					((x & 0x1) << 23 )
#define isrc_sel2(x)  					((x & 0x1) << 22 )
#define out_edn(x)  					((x & 0x3) << 20 )
#define idst_edn(x)  					((x & 0x3) << 18 )
#define isrc_edn(x)  					((x & 0x3) << 16 )
#define odmac_mode(x) 					((x & 0x7) << 12 )
#define iddma_mode(x)  					((x & 0x7) << 9 )
#define isdma_mode(x)  					((x & 0x7) << 6 )
#define odma_lev(x)  					((x & 0x3) << 4 )
#define iddma_lev(x)  					((x & 0x3) << 2 )
#define isdma_lev(x)  					((x & 0x3) << 0 )
/* 0x8004 */
#define yuv_sign(x)  					((x & 0x1) << 4 )
#define d_bpp4(x)  						((x & 0x3) << 2 )
#define s_bpp4(x)  						((x & 0x3) << 0 )
/* 0x8008 */
#define en_irq_wrap_high(x) 		    ((x & 0x1) << 5 ) 
#define en_irq_wrap_low(x) 		 		((x & 0x1) << 4 ) 
#define en_irq_odmadone(x) 		 		((x & 0x1) << 3 )
#define en_irq_rg_rqcmd(x)  			((x & 0x1) << 2 )
#define en_irq_cmdq_ful(x)  			((x & 0x1) << 1 )
#define en_irq_cmdq_emp(x)  			((x & 0x1) << 0 )
/* 0x800c */
#define wrap_high(x)  		            ((x & 0x1) << 22 )
#define wrap_low(x)  			        ((x & 0x1) << 21 )
#define st_exeopr(x)  					((x & 0x1) << 20 )
#define st_otdma(x)  					((x & 0x1) << 18 )
#define st_iddma(x)  					((x & 0x1) << 17 )
#define st_isdma(x)  					((x & 0x1) << 16 )
#define cmd_sp(x)  						((x & 0x1FF) << 4 )
#define irq_odmadone(x)  				((x & 0x1) << 3 )
#define wait_cmd(x)  					((x & 0x1) << 2 )
#define cmdq_full(x)  					((x & 0x1) << 1 )
#define cmdq_empty(x)  					((x & 0x1) << 0 )
/* 0x8020 */
#define obj_adr(x)  					((x & 0x7FFFFFF) << 0 )
#define frmbs_g(x)  					((x & 0x1FFFF) << 10 )
/* 0x8024 */
#define b_obj_dy(x)  				    ((x & 0x7FF)  << 13 )
#define obj_dy(x)  						((x & 0x1FFF) << 13 )
#define obj_dx(x)  						((x & 0x1FFF) << 0 )
/* 0x8028 */
#define dma_dx8(x)  					((x & 0x3FF)  << 13 )
#define gp_wid(x)	  					((x & 0x1FFF) << 0 )
/* 0x802c */
#define tx_nr8(x)  						((x & 0x3FF)  << 0 )
#define sc_y_g(x)  						((x & 0x1FFF) << 13 )
#define sc_x_pl_g(x)  					((x & 0x1FFF) << 0 )
/* 0x8038 */
#define cnt_main(x)  					((x & 0x3FF) << 18 )
#define cnt_sub(x)  					((x & 0xFF) << 10 )
#define srm_sel(x)  					((x & 0x3) << 8)
#define rst_vcf(x)  					((x & 0x1) << 7)
#define rst_hcf(x)  					((x & 0x1) << 6)
#define rst_pal(x)  					((x & 0x1) << 5)
#define rst_cmdq(x)  					((x & 0x1) << 4)
#define en_cmdq(x)  					((x & 0x1) << 2)
#define rst_dmacnt(x)  					((x & 0x1) << 1)
#define sw_rst(x)  						((x & 0x1) << 0)
/* 0x803c */
#define scale_wrap_enable(x)            ((x & 0x1) << 26)    //for 912&opr_cmd(7)
 /* t-scaling for 912&opr_cmd(7)*/
#define t_lu_dif(x)                     ((x & 0x3) << 24)
#define t_l_dif(x)                      ((x & 0x3) << 22)
#define t_dir(x)                        ((x & 0x3) << 20)
#define t_width(x)                      ((x & 0xF) << 16)

#define rotationsel(x)  				((x & 0x7) << 13 )
#define last_sub(x)  					((x & 0x1) << 12 )
#define opr_cmd(x)  					((x & 0xF) << 8 )
#define rop_func(x)  					((x & 0xFF) << 0 )
/* 0x8050 */
#define p_lower(x)  					((x & 0xFFFF) << 0 )
/* 0x8054 */
#define p_high(x)  						((x & 0xFFFF) << 0 )
/* 0x805c */
#define sr_i(x)  						((x & 0xFF) << 16 )
#define sr_n(x)  						((x & 0xFF) << 8 )
#define sr_m(x)  						((x & 0xFF) << 0 )
/* 0x8068 */
#define g2ddenflip(x)  					((x & 0x1) << 26)
#define g2dflipnum(x)  					((x & 0x1) << 25)
#define g2dflipaddress(x)	  			((x & 0x3FFFFF) << 3)
/* 0x806c */
#define updatedstalpha(x)  				((x & 0x1) << 18)
#define dst_usedefaultalpha(x)			((x & 0x1) << 17)
#define src_usedefaultalpha(x)			((x & 0x1) << 16)
#define dst_defaultalpha(x)  			((x & 0xFF) << 8)
#define src_defaultalpha(x)  			((x & 0xFF) << 0)
/* 0x8070 */
#define g2dflipwait(x)					((x & 0x1) << 26)
#define g2dresourceid(x)				((x & 0x3FFFF) << 8)
#define g2dflipaddresshigh(x)			((x & 0xFF) << 0)
/* 0x8074 */
#define scale_wrap0rdy(x)               (x & 0x1)             //for 912
#define scale_wrap1rdy(x)               ((x & 0x1) << 1)      //for 912
/* 0x8078 */
#define scale_wrap_l(x)                 (x & 0x1FFF)          //for 912
#define scale_wrap_h(x)                 ((x & 0x1FFF) << 13)  //for 912

//CRT and G2D flip exchange key
#define G2D_CRT_enable(x)               ((x & 0x1) << 0 )
#define G2D_usecomdq(x)                 ((x & 0x1) << 1 )
#define G2D_crtenable   	    		0x90005340
//G2D Register Defination
#define G2D_dmaintf						0x90008000
#define G2D_mode 	           			0x90008004
#define G2D_irqen 	         			0x90008008
#define G2D_status 	         			0x9000800c
#define G2D_src_start        			0x90008010 
#define G2D_src_dxdy 	       			0x90008014
#define G2D_src_wid 	     			0x90008018 //bitble mode
#define G2D_bb_src_txnr	     			0x9000801c //bitble mode
#define G2D_sc_src_orig 	  			0x9000801c //scaling mode
#define G2D_dst_start     			    0x90008020 
#define G2D_dst_dxdy         			0x90008024 //bitble mode
#define G2D_dst_wid        			    0x90008028 
#define G2D_bb_dst_txnr      			0x9000802c //bitble mode
#define G2D_sc_dst_orig      			0x9000802c //scaling mode
#define G2D_ctl              			0x90008038
#define G2D_operation        			0x9000803c
#define G2D_roppatlow        			0x90008050
#define G2D_roppathigh       			0x90008054
#define G2D_sc_h_ratio       			0x90008058 //scaling mode
#define G2D_sc_v_ratio       			0x9000805c  
#define G2D_prg_srm          			0x90008060
#define G2D_cmdq             			0x90008060
#define G2D_table_initial    			0x90008060
#define G2D_rev_id           			0x90008064
#define G2D_flip             			0x90008068
#define G2D_alpha_mode       			0x9000806C
#define G2D_flip_info        			0x90008070
#define G2D_wrap_rdy                    0x90008074 //for 912
#define G2D_wrap_threshold              0x90008078 //for 912
/*************************************************/

#define G2D_DMAC_INIT_MODE ((1 << 20) | (1 << 9) | 1)
#define G2D_DMA_MODE ((2 << 6) | (2 << 3) | 2)

#define G2D_DMA_REQ_LVL 0x15
#define G2D_IRQ_NUM 10


/* ch  8 */
#define dmac_init_mode_g2d_src_in    	0x90001880 
#define dmac_init_addr_g2d_src_in     	0x90001884
#define dmac_cur_mode_g2d_src_in      	0x90001888 
#define dmac_cur_addr_g2d_src_in      	0x9000188c 
/* ch  9 */
#define dmac_init_mode_g2d_dst_out    	0x90001890 
#define dmac_init_addr_g2d_dst_out    	0x90001894 
#define dmac_cur_mode_g2d_dst_out     	0x90001898 
#define dmac_cur_addr_g2d_dst_out     	0x9000189c 
/* ch 10 */
#define dmac_init_mode_g2d_dst_in     	0x900018a0 
#define dmac_init_addr_g2d_dst_in     	0x900018a4 
#define dmac_cur_mode_g2d_dst_in      	0x900018a8 
#define dmac_cur_addr_g2d_dst_in      	0x900018ac 

/* ch  8 */
#define dmac2_init_mode_g2d_src_in    	0x900018c0 
#define dmac2_init_addr_g2d_src_in    	0x900018c4
#define dmac2_cur_mode_g2d_src_in     	0x900018c8 
#define dmac2_cur_addr_g2d_src_in     	0x900018cc 
/* ch  9 */
#define dmac2_init_mode_g2d_dst_out   	0x900018d0 
#define dmac2_init_addr_g2d_dst_out   	0x900018d4 
#define dmac2_cur_mode_g2d_dst_out    	0x900018d8 
#define dmac2_cur_addr_g2d_dst_out    	0x900018dc 
/* ch 10 */
#define dmac2_init_mode_g2d_dst_in		0x900018e0 
#define dmac2_init_addr_g2d_dst_in    	0x900018e4 
#define dmac2_cur_mode_g2d_dst_in     	0x900018e8 
#define dmac2_cur_addr_g2d_dst_in     	0x900018ec 
#if 0
/* ch  8 */
#define dmac2_init_mode_g2d_src_in    0x90002880 
#define dmac2_init_addr_g2d_src_in    0x90002884
#define dmac2_cur_mode_g2d_src_in     0x90002888 
#define dmac2_cur_addr_g2d_src_in     0x9000288c 
/* ch  9 */
#define dmac2_init_mode_g2d_dst_out   0x90002890 
#define dmac2_init_addr_g2d_dst_out   0x90002894 
#define dmac2_cur_mode_g2d_dst_out    0x90002898 
#define dmac2_cur_addr_g2d_dst_out    0x9000289c 
/* ch 10 */
#define dmac2_init_mode_g2d_dst_in    0x900028a0 
#define dmac2_init_addr_g2d_dst_in    0x900028a4 
#define dmac2_cur_mode_g2d_dst_in     0x900028a8 
#define dmac2_cur_addr_g2d_dst_in     0x900028ac 
#endif
/**
   G2D Sub-block info. Each G2D operation can handle at most 4K bytes.
   Therefore, for operation that involves more than 4K bytes of data,
   this module shall divide the total chunk of data into smaller
   chunks of less than or equal to 4K bytes size.
*/
typedef struct {
    int num_sub_blocks;     ///< Total number of sub-blocks
    int sub_block_height;   ///< Each sub-block's height
    int next_sb_offset_src; ///< Byte offset to the next source sub-block
    int next_sb_offset_dst; ///< Byte offset to the next dest sub-block
    int last_sb_height;     ///< The last sub-block's height
} G2D_Subblock_Info;

/// Constants for G2D operating commands.
typedef enum {
    G2D_OPR_ALPBLN = 2,  ///< Alpha-blending
    G2D_OPR_MEMSET = 3,  ///< Memory Set
    G2D_OPR_MEMCPY = 5,  ///< Memory Copy
    G2D_OPR_MEMMIX = 6,  ///< Memory Mix (dst_out = dst_in + src_in)
    G2D_OPR_MEMSCL = 7,  ///< Scaling
} G2D_OPR;

typedef enum {
    G2D_BLK_NORMAL,    ///< Normal blk size
    G2D_BLK_BANK_SWAP, ///< Use half blk size for bank swapping operation
    G2D_BLK_PALETTE,   ///< Use 1/4 blk size for palette look-up operation
} G2D_BLK;

/// Constants for G2D command queue registers
typedef enum {
    G2D_CMDQ_ID_DMAINTF      = 0,
    G2D_CMDQ_ID_MODE         = 1,
    G2D_CMDQ_ID_IRQEN        = 2,
    G2D_CMDQ_ID_BB_SRC_START = 4,
    G2D_CMDQ_ID_SC_SRC_FRMBS = 4,
    G2D_CMDQ_ID_SRC_DXDY     = 5,
    G2D_CMDQ_ID_SRC_WID      = 6,
    G2D_CMDQ_ID_BB_SRC_TXNR  = 7,
    G2D_CMDQ_ID_SC_SRC_ORIG  = 7,
    G2D_CMDQ_ID_BB_DST_START = 8,
    G2D_CMDQ_ID_SC_DST_FRMBS = 8,
    G2D_CMDQ_ID_DST_DXDY     = 9,
    G2D_CMDQ_ID_DST_WID      = 0xa,
    G2D_CMDQ_ID_BB_DST_TXNR  = 0xb,
    G2D_CMDQ_ID_SC_DST_ORIG  = 0xb,
    G2D_CMDQ_ID_CTL          = 0xe,
    G2D_CMDQ_ID_OPERATION    = 0xf,
    G2D_CMDQ_ID_ROPPATLOW    = 0x14,
    G2D_CMDQ_ID_ROPPATHIGH   = 0x15,
    G2D_CMDQ_ID_SC_H_RATIO   = 0x16,
    G2D_CMDQ_ID_SC_V_RATIO   = 0x17,
    G2D_CMDQ_ID_FLIP         = 0x1a,
    G2D_CMDQ_ID_ALPHA_MODE   = 0x1b,
    G2D_CMDQ_ID_FLIP_II      = 0x1b,
} G2D_CMDQ_ID;

// P: pattern  S: source  D: destination
/// Raster operating function
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
    bool flag;
//T-scaling    
    bool       t_scale;
    cyg_uint8  t_lu_point;
    cyg_uint8  t_line_point;
    cyg_uint8  t_direction;
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
/**
   \brief 2-D memory copy from source object inside source plane to
          dest object inside the dest plane.
   
   \param dst Pointer to the first byte of dest object.
   \param src Pointer to the first byte of source object.
   \param val The 8-bit value to be set.
   \param dmac_sel DMA control
          dmac_sel[2] - (1) dst output data uses DMAC2
                        (0) dst output data uses DMAC0
          dmac_sel[1] - (1) dst input data uses DMAC2
                        (0) dst input data uses DMAC0
          dmac_sel[0] - (1) src input data uses DMAC2
                        (0) src input data uses DMAC0
   \param obj_width Width of the object to be copied
   \param obj_height Height of the object to be copied
   \param src_width Width of the source plane
   \param dst_width Width of the dest plane

   \note All units are in bytes. Can do 1-D copy--just make obj_width,
         src_width and dst_width the same and less than 2K; also make
         obj_width * obj_height equals to the length of data.
*/

int G2D_memcpy(G2D_data *data);
/**
   \brief 2-D memory set dest object inside the dest plane.
*/
int G2D_memset(G2D_data *data);

/**
   \brief Does 2-D alpha-blending on 2 planes. Only works on 4 byte/pixel
          data format (ARGB or AYUV).
   
   \param dst Pointer to the first byte of dest object. Each pixel in the
          object is in the AYUV or ARGB format, so this pointer is pointing
          at the alpha value of the upper left corner pixel of the object.
   \param src Pointer to the first byte of source object.
   \param dmac_sel DMA control
   \param mode Alpha-blending mode.
   \verbatim
          ALPBLN_MODE_CLEAR    (0) -- clear
          ALPBLN_MODE_SRC      (1) -- SRC
          ALPBLN_MODE_SRC_IN   (2) -- SRC in
          ALPBLN_MODE_SRC_OUT  (3) -- SRC out
          ALPBLN_MODE_SRC_OVER (4) -- SRC over
          ALPBLN_MODE_DST_IN   (5) -- DST in
          ALPBLN_MODE_DST_OUT  (6) -- DST out
          ALPBLN_MODE_DST_OVER (7) -- DST over
          ALPBLN_MODE_PRE_MUL  (8) -- Premultiply
          ALPBLN_MODE_ALP_EXT  (9) -- Alpha-extend
   \endverbatim
   \param obj_width Width of the object to be blended
   \param obj_height Height of the object to be blended
   \param src_width Width of the source plane
   \param dst_width Width of the dest plane
   \param updatedstalpha  write alpha back to destionation 1: use alpha output of alpha blending  ; 0: use original destination alpha
   \param dst_en_def_alpha  1: use default alpha to replace destination alpha to do alpha blending
   \param src_en_def_alpha  1: use default alpha to replace source alpha to do alpha blending; 
   \param dst_def_alpha  destination default alpha
   \param src_def_alpha  source default alpha


   \note All units are in bytes. 
*/
void G2D_alpha_blend(G2D_data *data);

/**
   \brief 2-D memory mix. Mixes source object and dest object and outputs
          result to dest object. Can specify the mixing function.
   
   \param dst Pointer to the first byte of dest object.
   \param src Pointer to the first byte of source object.
   \param dmac_sel DMA control
   \param ropf Raster operating function. Refer to \ref g2d_module.
   \param pattern The pattern used in the ROP function.
   \param obj_width Width of the object to be mixed
   \param obj_height Height of the object to be mixed
   \param src_width Width of the source plane
   \param dst_width Width of the dest plane

   \note All units are in bytes.
*/
int G2D_memmix(G2D_data *data);
/**
   \brief The G2D scaler. Source and dest. planes must be in 4 byte/pixel
          formats. Scales a source rectangular object from source plane to
          dest plane. Vertical and horizontal scaling ratios can be different.
          Max scale down is 255+255/256:1.
          Max scale up is 1:256.
   \param dst The address of the dest. plane (upper left corner). Must be
              1K byte aligned.
   \param src The address of the source plane (upper left corner). Must be
              1K byte aligned.
   \param src_x X-coord. of the upper left corner of source object.
   \param src_y Y-coord. of the upper left corner of source object.
   \param dst_x X-coord of where to put the scaled object in dest. plane
   \param dst_y Y-coord of where to put the scaled object in dest. plane
   \param vsr_i The integer part (I) of the vertical scaling ratio.
   \param vsr_n The numerator part (N) of the vertical scaling ratio.
   \param vsr_m The denominator part (M) of the vertical scaling ratio.
   \param hsr_i The integer part (I) of the horizontal scaling ratio.
   \param hsr_n The numerator part (N) of the horizontal scaling ratio.
   \param hsr_m The denominator part (M) of the horizontal scaling ratio.
   \param obj_width The width of the source rectangular object.
   \param obj_height The height of the source rectangular object.
   \param src_width The width of source plane.
   \param dst_width The width of dst plane.
   \* if you want to get the 1:1. 
        you can set vsr_i=vsr_n=vsr_m=0 hsr_i=hsr_n=hsr_m=0

   \note All units are in pixels.
 */
int G2D_memscl(G2D_data *data);
/** 
    \brief Map source object (1 byte/pixel) to dest. object (4 byte/pixel)
           using a SRAM palette look-up table.
    \param dst Pointer to the first byte of dst object.
    \param src Pointer to the first byte of src object.
    \note Use G2D_init_palette() to setup the palette.
 */
int G2D_memmap(G2D_data *data);

/**
   \brief Initializes the SRAM palette look-up table or scaling coeff tables
   \param tbl The  table.
   \param vert     1: palette table  :  Max_data can`t over 128
                   2: horizontal scaling table : Max_data = 32
                   3: vertical scaling table : Max_data = 32
   \param Max_data No. of palette entries. Max_data is 128.
*/
void G2D_init_tbl(cyg_uint32 tbl[], int vert, int max_data);

/**
   \brief Customized version of G2D_memcpy(). Adds a ROP function and a
          pattern to the basic memcpy operation.
   
   \param ropf Raster operating function. Refer to \ref g2d_module.
   \param pattern The pattern used in the ROP function.

   \note All units are in bytes.
*/
int G2D_cust_memcpy(G2D_data *data);

/**
   \brief Customized version of G2D_memset(). Adds a ROP function to the
          basic memset operation.
   
   \param ropf Raster operating function. Refer to \ref g2d_ropf.

   \note All units are in bytes.
*/
int G2D_cust_memset(G2D_data *data);

/**
   \brief The general memory alternation function. All memory-altering
          routines are based on it.
   
   \param opr_cmd Operating command.
          G2D_OPR_ALPBLN (2) -- Alpha-blending
          G2D_OPR_MEMSET (3) -- Memory Set
          G2D_OPR_MEMCPY (5) -- Memory Copy
          G2D_OPR_MEMMIX (6) -- Memory Mix (dst_out = dst_in + src_in)
   \param rate
          (0) -- source and dest planes are both 1 byte/pixel
          (1) -- source and dest planes are both 4 byte/pixel
          (2) -- source plane is 1 byte/pixel; dest plane is 4 byte/pixel
   \param rotat
   	   (0)disable
   	   (1)reserve
   	   (2)rotation 90 (counterclockwise)
   	   (3)rotation270
   	   (4)rotation 180
   	   (5)horizontal mirror
   	   (6)vertical mirror
   	   (7)reserve
   \retval 0 Success
           -1 Un-supported operating command
   \note All units are in bytes. Uses it directly for advanced operations.
*/
int G2D_memalt(cyg_uint8 opr_cmd, cyg_uint8 *dst, cyg_uint8 *src, cyg_uint8 dmac_sel, 
               int ropf, int pattern, int rate, int rotat, int obj_width, int obj_height, 
               int src_width, int dst_width); 
/**
   \brief The queued version of G2D_rotation()
*/
int G2D_rotation(G2D_data *data);
/**
   \brief flip set
   \param addr  framebuffer start address
   \param choose   framebuffer number choice 0 or 1 
   \\param on  if you want to turn on the flip please set it 1
*/
void G2D_flip_set(cyg_uint32 addr, cyg_uint8 choose, bool on);
/**
   \wrap mode : buffer exchange
*/
#ifdef CYG_HAL_CHIP_VERSION_910B
int G2D_wrap_flip(G2D_data *data);
#endif // ~CYG_HAL_CHIP_VERSION_910B


int G2D_memalt_repeat(cyg_uint8 opr_cmd, cyg_uint8 *dst, cyg_uint8 *src, cyg_uint8 dmac_sel, 
               int ropf, int pattern, int rate, int rotat, int obj_width, int obj_height,
               int src_width, int dst_width);
int G2D_memalt_repeat_q(cyg_uint8 opr_cmd, cyg_uint8 *dst, cyg_uint8 *src, cyg_uint8 dmac_sel, 
               int ropf, int pattern, int rate, int rotat, int obj_width, int obj_height,
               int src_width, int dst_width);
#endif
