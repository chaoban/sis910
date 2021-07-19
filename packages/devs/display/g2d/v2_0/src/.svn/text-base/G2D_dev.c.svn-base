/* < UPDATE NOTE >
 *1. 980304 - Add the sync function before the 2D function. 
 *
*/
/* < ANNOTATION >
 * 1. For 910B
 *    T - scaling and wrap scaling don`t have the comdq mode.
*/
#include <pkgconf/hal.h>
#include <cyg/hal/hal_io.h>       // IO macros
#include <cyg/hal/hal_if.h>       // Virtual vector support
#include <cyg/hal/hal_arch.h>     // Register state info
#include <cyg/hal/hal_intr.h>     // HAL interrupt macros
#include <stdio.h>  // sscanf
#include <cyg/devs/display/g2d/G2D_dev.h>
#include <memmanage/memmanage.h>

//***************************************************************************
//#define G2D_comq_enable 1
//#define __2d_debug_mode 1

#define DEBUG_addr 1
//***************************************************************************
#ifdef DEBUG_addr 
#define addr_printf(x)  diag_printf(x)
#else
#define addr_printf(x)
#endif

#define HAL_AND_UINT32(_reg_, _val_) (*((volatile CYG_WORD32 *)(_reg_)) &= (_val_) )
#define HAL_Value_Read_UINT32(_reg_)  *((volatile CYG_WORD32 *)(_reg_))

#define G2D_is_source_only_ropf(a) \
    ( (a==0x00) || (a==0x33) || (a==0xc0) || (a==0xcc) || (a==0xcf) ||(a==0xf0) || (a==0xff) )

#define G2D_is_dest_only_ropf(a) \
    ( (a==0x00) || (a==0x05) || (a==0x55) || (a==0x5a) || (a==0xaa) ||(a==0xf0) || (a==0xff) )

#define G2D_cmdq_add(id, val) do { \
        assert((id) >=0 && (id) <= 27); \
        while (((HAL_Value_Read_UINT32(G2D_status) >> 4) & 0x1ff) == 0) \
            ; \
        HAL_WRITE_UINT32(G2D_cmdq, ((id) << 27) | (val));\
} while (0)

// the scaling table
static cyg_uint32 g2d_sca_tbl[32] = {
0x063B07F8, 0x043B0AF7, 0x023B0CF7, 0x013B0EF6, 0xFF3B10F6, 0xFE3A13F5,
0xFD3915F5, 0xFB3818F5, 0xFA371AF5, 0xF9361DF4, 0xF8351FF4, 0xF73421F4,
0xF63224F4, 0xF63026F4, 0xF52F28F4, 0xF52C2AF5, 0xF52A2CF5, 0xF4282FF5,
0xF42630F6, 0xF42432F6, 0xF42134F7, 0xF41F35F8, 0xF41D36F9, 0xF51A37FA,
0xF51838FB, 0xF51539FD, 0xF5133AFE, 0xF6103BFF, 0xF60E3B01, 0xF70C3B02,
0xF70A3B04, 0xF8073B06,
};
/*g2d_function*/
static void g2d_program_sub_block(cyg_uint8 *dst, cyg_uint8 *src, 
                                  int s_obj_width, int s_obj_height,
                                  int d_obj_width, int d_obj_height,
                                  int src_width, int dst_width, int rate);
static void g2d_program_sub_block_q(cyg_uint8 *dst, cyg_uint8 *src, 
                                    int s_obj_width, int s_obj_height,
                                    int d_obj_width, int d_obj_height,
                                    int src_width, int dst_width, int rete);
static void g2d_start_operation(int opr);
static void g2d_queue_operation(int opr);
static void dissect_sub_block(int obj_width, int obj_height, int src_width,
                              int dst_width, G2D_Subblock_Info *, int bsize);
static cyg_uint32 magnify_ratio(cyg_uint32 in_n, cyg_uint32 in_m, 
                                cyg_uint32 *out_n, cyg_uint32 *out_m);
static int g2d_max_blk_size(G2D_BLK);
static void g2d_dmac_clear(void);

static cyg_interrupt G2D_inter_data;
static cyg_handle_t  G2D_inter_handle;
static cyg_devio_table_t G2D_devio;

inline void enable_g2d_irq(void);
inline void disable_g2d_irq(void);
inline void clear_g2d_irq(void);
void G2D_dsr(cyg_vector_t vect);
void G2D_isr(cyg_vector_t vect);
void lag_g2d(int x);
void read_2d_register(void);
void g2d_memory_sync(cyg_uint8 *dst, cyg_uint16 obj_width, cyg_uint16 obj_height,
                     cyg_uint16 dst_pitch);

int (*memalt_type)(cyg_uint8 opr_cmd, cyg_uint8 *dst, cyg_uint8 *src, 
                   cyg_uint8 dmac_sel, int ropf, int pattern, int rate, 
                   int rotat, int obj_width, int obj_height, 
                   int src_width, int dst_width);

static bool G2D_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo G2D_lookup(struct cyg_devtab_entry **tab, 
                            struct cyg_devtab_entry *sub_tab,
                            const char *name);
static Cyg_ErrNo G2D_set_config(cyg_io_handle_t handle, cyg_uint32 key, 
                                void *buf, cyg_uint32 *len);

inline void enable_g2d_irq(void)
{
}
inline void disable_g2d_irq(void)
{
}
inline void clear_g2d_irq(void)
{
}
void G2D_isr(cyg_vector_t vect)
{
    clear_g2d_irq();
}
void G2D_dsr(cyg_vector_t vect)
{
}
static bool G2D_init(struct cyg_devtab_entry *tab)
{
    /*scaling table*/
    G2D_init_tbl(g2d_sca_tbl, 2, 32);
    G2D_init_tbl(g2d_sca_tbl, 3, 32);
    cyg_drv_interrupt_mask(G2D_IRQ_NUM);
    cyg_drv_interrupt_create(G2D_IRQ_NUM,
                            32,        // priority
                             0,         // data
                             &G2D_isr,                             
                             &G2D_dsr,
                             &G2D_inter_handle,
                             &G2D_inter_data);
    cyg_drv_interrupt_attach(G2D_inter_handle);
    cyg_drv_interrupt_unmask(G2D_IRQ_NUM);
    return true;
}
static Cyg_ErrNo G2D_lookup(struct cyg_devtab_entry **tab, 
                            struct cyg_devtab_entry *sub_tab,
                            const char *name)
{
    return ENOERR;
}
static Cyg_ErrNo G2D_set_config(cyg_io_handle_t handle, cyg_uint32 key, void *buf, cyg_uint32 *len)
{
    G2D_data *getbuf = (G2D_data *)buf;
    switch (key) {
        case 0:
            G2D_flip_set(getbuf->flip_data.addr, getbuf->flip_data.choose, getbuf->flip_data.on);
            break;
        case 1:
#ifdef CYG_HAL_CHIP_VERSION_910B
            G2D_wrap_flip(getbuf);
#endif
            break;
        case 2:             
            G2D_alpha_blend(getbuf);
            break;
        case 3:
            G2D_memset(getbuf);
            break;
        case 5:
            G2D_memcpy(getbuf);
            break;
        case 6:
            G2D_memmix(getbuf);
            break;
        case 7:
            G2D_memscl(getbuf);
            break;
        case 8:
            G2D_rotation(getbuf);
            break;
        case 9:
            G2D_init_tbl(getbuf->pal_table, 1, getbuf->pal_data);
            G2D_memmap(getbuf);
            break;
        default:
            break;
        }
    return ENOERR;
}
static DEVIO_TABLE(G2D_devio,
                   NULL,
                   NULL,
                   NULL,
                   NULL,
                   G2D_set_config
                  );

static DEVTAB_ENTRY(G2D_devtab_entry, 
             "/dev/display/g2d",
             0,
             &G2D_devio, 
             G2D_init, 
             G2D_lookup,      // Execute this when device is being looked up
             0);

#ifdef CYG_HAL_CHIP_VERSION_910B
int G2D_wrap_flip(G2D_data *data)
{
    cyg_uint32 a = 0;
    cyg_uint32 opr = scale_wrap_enable(1) | opr_cmd(7);
    if(data->scaling_data.wrap_count == 0 || data->scaling_data.wrap_count == 1)
    {
        if(data->scaling_data.wrap_high_ready)
        {
            HAL_WRITE_UINT32(G2D_wrap_rdy, scale_wrap1rdy(1) | scale_wrap0rdy(0));
        }
        else
        {
            // after the low buffer ready, we can start the opr. 
            HAL_WRITE_UINT32(G2D_operation, opr);
            HAL_WRITE_UINT32(G2D_wrap_rdy, scale_wrap1rdy(0) | scale_wrap0rdy(1));
        }
    }
    else
    {
        if(data->scaling_data.wrap_high_ready)
        {
            // write down
            HAL_WRITE_UINT32(G2D_wrap_rdy, scale_wrap1rdy(1) | scale_wrap0rdy(0));
            // wait low
            do{HAL_READ_UINT32(G2D_status, a);}
            while(!(a >> 21) & 0x1);
            // reset low
            a &= ~0x200000;
        }
        else
        {
            // write down
            HAL_WRITE_UINT32(G2D_wrap_rdy, scale_wrap1rdy(0) | scale_wrap0rdy(1));
            // wait high
            do{HAL_READ_UINT32(G2D_status, a);}
            while(!(a >> 22) & 0x1);
            // reset high
            a &= ~0x400000;             
        }
        HAL_WRITE_UINT32(G2D_status, a);
    }
    data->scaling_data.wrap_count = data->scaling_data.wrap_count + 1; 
    // at last time 
    if(data->scaling_data.wrap_count == data->scaling_data.wrap_total)
    {
        while ( (HAL_Value_Read_UINT32(G2D_status) & 0x8) == 0 ) { // polls odma done
        }
        HAL_WRITE_UINT32(G2D_status, irq_odmadone(0)); // clears odma done irq
        //HAL_WRITE_UINT32(G2D_ctl, sw_rst(1));
    }
    return 0;
}
#endif

int G2D_rotation(G2D_data *data)
{
    G2D_memalt(5, data->dst, data->src, data->dmac_sel, 0, 0, data->rate, data->rotat, 
                data->obj_width, data->obj_height, data->src_width, data->dst_width);
    return 0;
}

int G2D_memcpy(G2D_data *data)
{
/*
    G2D_cust_memcpy(data->dst, data->src, data->dmac_sel, ROPF_S, 0, data->obj_width, data->obj_height,
                    data->src_width, data->dst_width);
*/
    data->ropf = ROPF_S;
    G2D_cust_memcpy(data);
    return 0;
}

/* Sets a rectanglar block of memory to a value. */
int G2D_memset(G2D_data *data)  //must set pattern
{
    data->ropf = ROPF_P;
    G2D_cust_memset(data);
/*
    G2D_cust_memset(data->dst, data->pattern, data->dmac_sel, ROPF_P,data->obj_width, 
                    data->obj_height, data->dst_width);
*/
    return 0;
}

int G2D_memmix(G2D_data *data)
{
    G2D_memalt(6, data->dst, data->src, data->dmac_sel, data->ropf, data->pattern,
            0, 0, data->obj_width, data->obj_height, data->src_width, data->dst_width);
    return 0;
}

int G2D_memscl(G2D_data *data)
{
    cyg_uint32 dst_frmb = data->scaling_data.dst_frmb;
    cyg_uint32 src_frmb = data->scaling_data.src_frmb;
    cyg_uint32 src_x = data->scaling_data.src_x;
    cyg_uint32 src_y = data->scaling_data.src_y;
    cyg_uint32 dst_x = data->scaling_data.dst_x;
    cyg_uint32 dst_y = data->scaling_data.dst_y;
    cyg_uint32 vsr_i = data->scaling_data.vsr_i;
    cyg_uint32 vsr_n = data->scaling_data.vsr_n;
    cyg_uint32 vsr_m = data->scaling_data.vsr_m;
    cyg_uint32 hsr_i = data->scaling_data.hsr_i;
    cyg_uint32 hsr_n = data->scaling_data.hsr_n;
    cyg_uint32 hsr_m = data->scaling_data.hsr_m;
    int obj_width = data->obj_width;
    int obj_height = data->obj_height;
    int src_width = data->src_width;
    int dst_width = data->dst_width;
    g2d_dmac_clear();
    int opr = opr_cmd(7);
    cyg_uint32 a = 0;
    cyg_uint32 vi = 0, hi = 0,vn = 0, vm = 0, hn = 0, hm = 0;
    
    if ((vsr_i == 0) && (vsr_n == 0) && (vsr_m == 0)) {
        vsr_i = 1;
    }
    if ((hsr_i == 0) && (hsr_n == 0) && (hsr_m == 0)) {
        hsr_i = 1;
    }
    vi = vsr_i + magnify_ratio(vsr_n, vsr_m, &vn, &vm);
    if (vi > 255) {
        //convert to max ratio: 255 + 255/256
        vi = 255;
        vn = 255;
        vm = 0; // means 256 actually
    }
    hi = hsr_i + magnify_ratio(hsr_n, hsr_m, &hn, &hm);
    if (hi > 255) {
        hi = 255;
        hn = 255;
        hm = 0;
    }
#ifdef CYG_HAL_CHIP_VERSION_910B
    /* T scaling */
    if(data->scaling_data.t_scale != false)
    {
        opr |= t_lu_dif(data->scaling_data.t_lu_point) | 
               t_l_dif(data->scaling_data.t_line_point)|
               t_dir(data->scaling_data.t_direction)   | 
               t_width(data->scaling_data.t_width);
    }
    
    /* wrap mode initial*/
    if(data->scaling_data.wrap_mode != false)
    {
        opr |= scale_wrap_enable(1);
        HAL_WRITE_UINT32(G2D_wrap_threshold, \
                        (scale_wrap_l(data->scaling_data.wrap_low_height)) | 
                        (scale_wrap_h(data->scaling_data.wrap_high_height)));//wrap
        HAL_WRITE_UINT32(G2D_status, wrap_high(0) | wrap_low(0)); 
        HAL_WRITE_UINT32(G2D_wrap_rdy, scale_wrap1rdy(0) | scale_wrap0rdy(0));
    }
#endif
    //  1 --> for Jpeg use
    a = (data->scaling_data.flag == 0) ? (0x00002495) : (0x000024d5);
#ifdef G2D_comq_enable
    HAL_WRITE_UINT32(G2D_ctl, en_cmdq(1) | rst_cmdq(1));
    HAL_WRITE_UINT32(G2D_ctl, en_cmdq(1) | srm_sel(0));
    G2D_cmdq_add(G2D_CMDQ_ID_DMAINTF, a);
    G2D_cmdq_add(G2D_CMDQ_ID_MODE, 5); // scaling only works in 4 byte/pixel format
    G2D_cmdq_add(G2D_CMDQ_ID_SC_SRC_FRMBS, frmbs_g(src_frmb >> 10));
    G2D_cmdq_add(G2D_CMDQ_ID_SRC_DXDY, (obj_dy(obj_height)|obj_dx(obj_width)));
    G2D_cmdq_add(G2D_CMDQ_ID_SRC_WID, gp_wid(src_width));
    G2D_cmdq_add(G2D_CMDQ_ID_SC_SRC_ORIG, (sc_y_g(src_y)|sc_x_pl_g(src_x)));
    G2D_cmdq_add(G2D_CMDQ_ID_SC_DST_FRMBS, frmbs_g(dst_frmb >> 10));    
    G2D_cmdq_add(G2D_CMDQ_ID_DST_WID, gp_wid(dst_width));
    G2D_cmdq_add(G2D_CMDQ_ID_SC_DST_ORIG, (sc_y_g(dst_y) | sc_x_pl_g(dst_x)));
    G2D_cmdq_add(G2D_CMDQ_ID_SC_H_RATIO, (sr_i(hi) | sr_n(hn) | sr_m(hm)));
    G2D_cmdq_add(G2D_CMDQ_ID_SC_V_RATIO, (sr_i(vi) | sr_n(vn) | sr_m(vm)));
    G2D_cmdq_add(G2D_CMDQ_ID_IRQEN, en_irq_odmadone(1));
    g2d_queue_operation(opr);
#else
    HAL_WRITE_UINT32(G2D_dmaintf, a); 
    HAL_WRITE_UINT32(G2D_mode, 5); // scaling only works in 4 byte/pixel format
    // src addr must be 1k byte aligned
    HAL_WRITE_UINT32(G2D_src_start, frmbs_g(src_frmb >> 10));
    HAL_WRITE_UINT32(G2D_src_dxdy, (obj_dy(obj_height) | obj_dx(obj_width)));
    HAL_WRITE_UINT32(G2D_src_wid, gp_wid(src_width));
    HAL_WRITE_UINT32(G2D_sc_src_orig, (sc_y_g(src_y) | sc_x_pl_g(src_x)));
    // src addr must be 1k byte aligned   
    HAL_WRITE_UINT32(G2D_dst_start, frmbs_g(dst_frmb >> 10));    
    HAL_WRITE_UINT32(G2D_dst_wid, gp_wid(dst_width));
    HAL_WRITE_UINT32(G2D_sc_dst_orig, (sc_y_g(dst_y) | sc_x_pl_g(dst_x)));
    HAL_WRITE_UINT32(G2D_sc_h_ratio, (sr_i(hi) | sr_n(hn) | sr_m(hm)));
    HAL_WRITE_UINT32(G2D_sc_v_ratio, (sr_i(vi) | sr_n(vn) | sr_m(vm)));   
#ifdef CYG_HAL_CHIP_VERSION_910B
    if (data->scaling_data.wrap_mode != false) 
    {    
        HAL_WRITE_UINT32(G2D_irqen, (en_irq_rg_rqcmd(1) | en_irq_odmadone(1) | 
                         en_irq_wrap_high(1) | en_irq_wrap_low(1)));
    }
    else
    {
        g2d_start_operation(opr);
        HAL_WRITE_UINT32(G2D_ctl, sw_rst(1));
    }
#else
    g2d_start_operation(opr);
    HAL_WRITE_UINT32(G2D_ctl, sw_rst(1));    
#endif
#endif
    return 1;
}
// map a 1 byte/pixel object to 4 byte/pixel with a LUT.
// all units in byte
int G2D_memmap(G2D_data *data)
{
    return G2D_memalt(G2D_OPR_MEMCPY, data->dst, data->src, data->dmac_sel, ROPF_S, 0, 
            2, 0, data->obj_width, data->obj_height, data->src_width, data->dst_width);
}

void G2D_init_tbl(cyg_uint32 tbl[], int vert,int max_data)
{
    //assert(max_data >= 1 && max_data <= 256);
    int i = 0, count = 32;
    switch(vert)
    {
        case 1:
            count = max_data;
            HAL_WRITE_UINT32(G2D_ctl, rst_pal(1));          
            break;
        case 2:
            HAL_WRITE_UINT32(G2D_ctl, rst_hcf(1));
            break;
        case 3: 
            HAL_WRITE_UINT32(G2D_ctl, rst_vcf(1));
            break;
        default:
            break;
    }
    HAL_WRITE_UINT32(G2D_ctl, srm_sel(vert));
    for (i = 0; i < count; i++) {
        HAL_WRITE_UINT32(G2D_table_initial, tbl[i]);
    }    
}
void G2D_flip_set(cyg_uint32 addr, cyg_uint8 choose, bool on)
{
    //QQ --> g2dresourceid
    HAL_WRITE_UINT32(G2D_crtenable, G2D_CRT_enable(1) | G2D_usecomdq(1));
    G2D_cmdq_add(G2D_CMDQ_ID_FLIP, (g2ddenflip(on)|g2dflipnum(choose)|g2dflipaddress(addr)));
    G2D_cmdq_add(G2D_CMDQ_ID_FLIP_II, (g2dflipwait(1)|g2dresourceid(0)|g2dflipaddresshigh((addr>>25))));
}
/* customized memcpy; uses raster operating function and a pattern */

int G2D_cust_memcpy(G2D_data *data)
{
    if (!G2D_is_source_only_ropf(data->ropf)) {
        data->ropf = ROPF_S;
    }
    return G2D_memalt(5, data->dst, data->src, data->dmac_sel, data->ropf, data->pattern, 0,0,
                      data->obj_width, data->obj_height, data->src_width,data->dst_width);
}

/* customized memset; uses raster operating function and a pattern */
int G2D_cust_memset(G2D_data *data)
{
    if (!G2D_is_dest_only_ropf(data->ropf)) {
        data->ropf = ROPF_P;
    }
    return G2D_memalt(3, data->dst, 0, data->dmac_sel, data->ropf, data->pattern,
                1, 0, data->obj_width, data->obj_height, 0, data->dst_width);
}

/* The function that memory-altering routines build on. */
/* rate: 0 => 1 byte/pixel to 1 byte/pixel
         1 => 4 byte/pixel to 4 byte/pixel
   Note: units are in pixels
*/
int G2D_memalt(cyg_uint8 opr_cmd, cyg_uint8 *dst, cyg_uint8 *src, cyg_uint8 dmac_sel, 
                   int ropf, int pattern, int rate, int rotat, int obj_width, int obj_height,
                   int src_width, int dst_width) 
{
    g2d_memory_sync(src, obj_width, obj_height, src_width); 
    
#ifdef G2D_comq_enable
    memalt_type = G2D_memalt_repeat_q;
#else
    memalt_type = G2D_memalt_repeat;
#endif

    int max_blk_size = 256; //pixel 
    int max_blk_size_byte = max_blk_size << 2;
    int max_blk_size_QQ = 4096;
    int max_dst_blk_size_byte = max_blk_size_byte;
    
    if(rate == 2)
    {
        max_blk_size_byte = max_blk_size;
        max_dst_blk_size_byte = max_blk_size_byte << 2;
        max_blk_size_QQ = 1024;
    }
    
    cyg_uint32 dst2 = (cyg_uint32)dst;
    cyg_uint32 src2 = (cyg_uint32)src;
    int dst_pitch = dst_width << 2;//(pixel 2 byte)
    int obj_pitch = obj_width << 2;
    
    if((obj_pitch * obj_height) <= max_blk_size_QQ)
    {
        (*memalt_type)(opr_cmd, dst, src, dmac_sel, 
        ropf, pattern, rate, rotat, obj_width, obj_height, src_width, dst_width);
    }
    else
    {
        if(rotat != 0)
        {
            max_blk_size = 32;
            max_blk_size_byte = max_blk_size << 2;
        }
        int obj_a_width   = max_blk_size;
        int obj_b_width   = obj_width % max_blk_size;
        int width_element = obj_width / max_blk_size;
        int i = 0;
        if(rotat == 0)
        {
            for(i = 0; i < width_element; i++)
            {
                (*memalt_type)(opr_cmd, (cyg_uint8*)dst2, (cyg_uint8*)src2, dmac_sel, ropf,
                    pattern, rate, rotat, obj_a_width, obj_height, src_width, dst_width);
                dst2 += max_dst_blk_size_byte;
                src2 += max_blk_size_byte;
            }
            if(obj_b_width != 0) // the last one
            {
                (*memalt_type)(opr_cmd, (cyg_uint8*)dst2, (cyg_uint8*)src2, dmac_sel, ropf,
                    pattern, rate, rotat, obj_b_width, obj_height, src_width, dst_width);             
            }   
        }
        else if(rotat == 6)
        {
            for(i = 0; i < width_element; i++)
            {
                (*memalt_type)(opr_cmd, (cyg_uint8*)dst2, (cyg_uint8*)src2, dmac_sel, ropf,
                    pattern, rate, rotat, obj_a_width, obj_height, src_width, dst_width);
                dst2 += max_blk_size_byte;
                src2 += max_blk_size_byte;
            }
            if(obj_b_width != 0) // the last one
            {
                (*memalt_type)(opr_cmd, (cyg_uint8*)dst2, (cyg_uint8*)src2, dmac_sel, ropf,
                    pattern, rate, rotat, obj_b_width, obj_height, src_width, dst_width);             
            }   
        }
        else if(rotat == 2)
        {
            dst2 += (obj_width - max_blk_size) * dst_pitch;
            for(i = 0; i < width_element; i++)
            {
                (*memalt_type)(opr_cmd, (cyg_uint8*)dst2, (cyg_uint8*)src2, dmac_sel, ropf,
                    pattern, rate, rotat, obj_a_width, obj_height, src_width, dst_width);
                dst2 -= max_blk_size * dst_pitch;
                src2 += max_blk_size_byte;
            }
            if(obj_b_width != 0) // the last one
            {
                dst2 += (max_blk_size - obj_b_width) * dst_pitch;
                (*memalt_type)(opr_cmd, (cyg_uint8*)dst2, (cyg_uint8*)src2, dmac_sel, ropf, 
                    pattern, rate, rotat, obj_b_width, obj_height, src_width, dst_width);
            }
        }
        else if(rotat == 3)
        {
            for(i = 0; i < width_element; i++)
            {
                (*memalt_type)(opr_cmd, (cyg_uint8*)dst2, (cyg_uint8*)src2, dmac_sel, ropf, 
                    pattern, rate, rotat, obj_a_width, obj_height, src_width, dst_width);
                dst2 += max_blk_size * dst_pitch;
                src2 += max_blk_size_byte;
            }
            if(obj_b_width != 0) // the last one
            {
                (*memalt_type)(opr_cmd, (cyg_uint8*)dst2, (cyg_uint8*)src2, dmac_sel, ropf, 
                    pattern, rate, rotat, obj_b_width, obj_height, src_width, dst_width);
            }
        }
        else if(rotat == 4 || rotat == 5)
        {
            dst2 += obj_pitch - max_blk_size_byte;
            for(i = 0; i < width_element; i++)
            {
                (*memalt_type)(opr_cmd, (cyg_uint8*)dst2, (cyg_uint8*)src2, dmac_sel, ropf, 
                    pattern, rate, rotat, obj_a_width, obj_height, src_width, dst_width);
                dst2 -= max_blk_size_byte;
                src2 += max_blk_size_byte;
            }
            if(obj_b_width != 0) // the last one
            {   
                dst2 += (max_blk_size_byte - (obj_b_width << 2));
                (*memalt_type)(opr_cmd, (cyg_uint8*)dst2, (cyg_uint8*)src2, dmac_sel, ropf,
                    pattern, rate, rotat, obj_b_width, obj_height, src_width, dst_width);
            }
        }
    }
    
    return 0;
}
int G2D_memalt_repeat(cyg_uint8 opr_cmd, cyg_uint8 *dst, cyg_uint8 *src, cyg_uint8 dmac_sel, 
               int ropf, int pattern, int rate,int rotat, int obj_width, int obj_height,
               int src_width, int dst_width) 
{
    int sb_d_obj_w = 0;
    int sb_d_obj_h = 0;
    cyg_uint32 a = 0;
    if ((opr_cmd != G2D_OPR_MEMCPY) && (opr_cmd != G2D_OPR_MEMSET) && 
        (opr_cmd != G2D_OPR_MEMMIX) && (opr_cmd != G2D_OPR_ALPBLN)) {
        diag_printf("G2D: Un-support memory operation!\n");
        return -1;
    }
    if(rate != 2)
    {
        obj_width = obj_width << 2;
        src_width = src_width << 2;
    }
    dst_width = dst_width << 2;

    int bank_swap = dmac_sel & 0xf0;
    g2d_dmac_clear();
    a = (rotat == 0) ? (0x00002495) : (0x00152495); // palette must be 0x2495
	HAL_WRITE_UINT32(G2D_dmaintf, a);

    if (rate == 0)
        HAL_WRITE_UINT32(G2D_mode, 0);
    else if (rate == 1)
        HAL_WRITE_UINT32(G2D_mode, 5);
    else if (rate == 2)
        HAL_WRITE_UINT32(G2D_mode, 4);
    else
        assert(0);

    HAL_WRITE_UINT32(G2D_irqen, 0x8);    

    int btype;
    if (bank_swap)
        btype = G2D_BLK_BANK_SWAP;
    else if (rate == 2)
        btype = G2D_BLK_PALETTE;
    else
        btype = G2D_BLK_NORMAL;

    int max_blk_size = g2d_max_blk_size(btype);
    G2D_Subblock_Info sb = {0};

    sb.num_sub_blocks = 1;
    sb.last_sb_height = obj_height;
    if (obj_width * obj_height > max_blk_size) {
        dissect_sub_block(obj_width, obj_height, src_width, dst_width, &sb, max_blk_size); 
    }
    int i = 0;
    int opr= (last_sub(1) | rotationsel(rotat) | opr_cmd(opr_cmd) | rop_func(ropf));
    
    int self_max_blk_size = max_blk_size >> 1;
    
    HAL_WRITE_UINT32(G2D_roppatlow, p_lower(pattern));
    HAL_WRITE_UINT32(G2D_roppathigh, p_high(pattern >> 16));

    //diag_printf("# sub-blocks %d\n", sb.num_sub_blocks);
    if (sb.num_sub_blocks == 1) {
        if(rotat == 2 || rotat == 3)
        {
            sb_d_obj_h = obj_width >> 2;
            sb_d_obj_w = sb.last_sb_height << 2;
            g2d_program_sub_block((cyg_uint8*)dst, (cyg_uint8*)src, obj_width , sb.last_sb_height, 
                sb_d_obj_w, sb_d_obj_h, src_width, dst_width, rate);
        }
        else
        {
            g2d_program_sub_block((cyg_uint8*)dst, (cyg_uint8*)src, obj_width , sb.last_sb_height, 
                obj_width, sb.last_sb_height, src_width, dst_width, rate);
            
        }
        g2d_start_operation(opr);
    }
    else if (sb.num_sub_blocks > 1) 
    {
        cyg_uint32 dst2 = (cyg_uint32)dst;
        if (bank_swap) {
            dst2 += self_max_blk_size;
        }
        if(rotat == 0 || rotat == 5)
        {
            g2d_program_sub_block((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width, 
                sb.sub_block_height, obj_width, sb.sub_block_height, src_width, dst_width, rate);
            for (i = 0; i < sb.num_sub_blocks - 1; i++) 
            {
                g2d_start_operation(opr);    
                if (opr_cmd != G2D_OPR_MEMSET) 
                {
                    src += sb.next_sb_offset_src;
                    HAL_WRITE_UINT32(G2D_src_start, obj_adr((cyg_uint32)src));
                    //diag_printf("src addr : %08x\n", (cyg_uint32)src);
                }
                dst2 += sb.next_sb_offset_dst;
                if (bank_swap) 
                {
                    if ((cyg_uint32)src & self_max_blk_size) // 4n + 2
                        dst2 -= self_max_blk_size;
                    else // 4n
                        dst2 += self_max_blk_size;
                }
                HAL_WRITE_UINT32(G2D_dst_start, obj_adr(dst2));
            }
            g2d_program_sub_block((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width, 
                sb.last_sb_height, obj_width, sb.last_sb_height, src_width, dst_width, rate);
        }
        else if(rotat == 2)
        {
            sb_d_obj_w = sb.sub_block_height << 2;
            sb_d_obj_h = obj_width >> 2;
            g2d_program_sub_block((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width,
                sb.sub_block_height, sb_d_obj_w, sb_d_obj_h, src_width, dst_width, rate);
            for (i = 0; i < sb.num_sub_blocks - 1; i++) 
            {
                g2d_start_operation(opr);    
                src += sb.next_sb_offset_src;
                HAL_WRITE_UINT32(G2D_src_start, obj_adr((cyg_uint32)src));
                dst2 += sb_d_obj_w;
                if (bank_swap) 
                {
                    if ((cyg_uint32)src & self_max_blk_size) // 4n + 2
                        dst2 -= self_max_blk_size;
                    else // 4n
                        dst2 += self_max_blk_size;
                }
                HAL_WRITE_UINT32(G2D_dst_start, obj_adr((cyg_uint32)dst2));
            }
            sb_d_obj_w = sb.last_sb_height << 2; 
            g2d_program_sub_block((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width, 
                sb.last_sb_height, sb_d_obj_w, sb_d_obj_h, src_width, dst_width, rate);
        }
        else if(rotat == 3)
        {
            sb_d_obj_w = sb.sub_block_height << 2;
            sb_d_obj_h = obj_width >> 2;
            dst2 += ((obj_height - sb.sub_block_height) << 2);
            g2d_program_sub_block((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width, 
                sb.sub_block_height, sb_d_obj_w, sb_d_obj_h, src_width, dst_width, rate);
            for (i = 0; i < sb.num_sub_blocks - 1; i++) 
            {
                g2d_start_operation(opr);    
                src += sb.next_sb_offset_src;
                HAL_WRITE_UINT32(G2D_src_start, obj_adr((cyg_uint32)src));
                dst2 -= sb_d_obj_w;
                if (bank_swap) 
                {
                    if ((cyg_uint32)src & self_max_blk_size) // 4n + 2
                        dst2 -= self_max_blk_size;
                    else // 4n
                        dst2 += self_max_blk_size;
                }
                HAL_WRITE_UINT32(G2D_dst_start, obj_adr((cyg_uint32)dst2));
            }
            sb_d_obj_w = sb.last_sb_height << 2; 
            dst2 += ((sb.sub_block_height - sb.last_sb_height) << 2);
            g2d_program_sub_block((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width,
                sb.last_sb_height, sb_d_obj_w, sb_d_obj_h, src_width, dst_width, rate);
        }
        else if(rotat == 4 || rotat == 6)
        {
            dst2 += (obj_height - sb.sub_block_height) * dst_width;
            g2d_program_sub_block((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width,sb.sub_block_height,
                obj_width, sb.sub_block_height, src_width, dst_width, rate);
            for (i = 0; i < sb.num_sub_blocks - 1; i++) 
            {
                g2d_start_operation(opr);    
                src += sb.next_sb_offset_src;
                HAL_WRITE_UINT32(G2D_src_start, obj_adr((cyg_uint32)src));
                dst2 -= sb.next_sb_offset_dst;
                if (bank_swap) 
                {
                    if ((cyg_uint32)src & self_max_blk_size) // 4n + 2
                        dst2 -= self_max_blk_size;
                    else // 4n
                        dst2 += self_max_blk_size;
                }
                HAL_WRITE_UINT32(G2D_dst_start, obj_adr((cyg_uint32)dst2));
            }
            dst2 += (sb.sub_block_height - sb.last_sb_height) * dst_width;
            g2d_program_sub_block((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width, 
                sb.last_sb_height, obj_width, sb.last_sb_height, src_width, dst_width, rate);
        }
        g2d_start_operation(opr);
    }
    HAL_WRITE_UINT32(G2D_irqen,en_irq_odmadone(0));
    return 0;
}

/* Similar to G2D_memalt, but use command queue */ 
int G2D_memalt_repeat_q(cyg_uint8 opr_cmd, cyg_uint8 *dst, cyg_uint8 *src, 
                        cyg_uint8 dmac_sel, int ropf, int pattern, int rate,
                        int rotat, int obj_width, int obj_height, int src_width, int dst_width) 
{
    int sb_d_obj_w = 0;
    int sb_d_obj_h = 0;
    cyg_uint32 a = 0;
    if ((opr_cmd != G2D_OPR_MEMCPY) && (opr_cmd != G2D_OPR_MEMSET) && 
        (opr_cmd != G2D_OPR_MEMMIX) && (opr_cmd != G2D_OPR_ALPBLN)) {
        diag_printf("G2D: Un-support memory operation!\n");
        return -1;
    }
    if(rate != 2)
    {
        obj_width=obj_width << 2;
        src_width=src_width << 2;
    }
    dst_width=dst_width << 2;

    int bank_swap = dmac_sel & 0xf0;

    g2d_dmac_clear();  
    
    HAL_WRITE_UINT32(G2D_ctl, en_cmdq(1) | rst_cmdq(1));
    HAL_WRITE_UINT32(G2D_ctl, en_cmdq(1) | srm_sel(0));

    a = (rotat == 0) ? (0x00002495) : (0x00152495);
    G2D_cmdq_add(G2D_CMDQ_ID_DMAINTF, a);

    if (rate == 0)
        G2D_cmdq_add(G2D_CMDQ_ID_MODE, 0);
    else if (rate == 1)
        G2D_cmdq_add(G2D_CMDQ_ID_MODE, 5);
    else if (rate == 2)
        G2D_cmdq_add(G2D_CMDQ_ID_MODE, 4);
    else
        assert(0);

    int btype = 0;
    if (bank_swap)
        btype = G2D_BLK_BANK_SWAP;
    else if (rate == 2)
        btype = G2D_BLK_PALETTE;
    else
        btype = G2D_BLK_NORMAL;

    int max_blk_size = g2d_max_blk_size(btype);

    G2D_Subblock_Info sb = {0};
    sb.num_sub_blocks = 1;
    sb.last_sb_height = obj_height;
    if (obj_width * obj_height > max_blk_size) {
        dissect_sub_block(obj_width, obj_height, src_width, dst_width, 
                          &sb, max_blk_size);
    }
    int i = 0;
    int opr= (last_sub(1) | rotationsel(rotat) | opr_cmd(opr_cmd) | rop_func(ropf));
    
    int self_max_blk_size = max_blk_size >> 1;
    G2D_cmdq_add(G2D_CMDQ_ID_ROPPATLOW, p_lower(pattern));
    G2D_cmdq_add(G2D_CMDQ_ID_ROPPATHIGH, p_high(pattern >> 16));
    if (sb.num_sub_blocks == 1) {
        if(rotat == 2 || rotat == 3)
        {
            sb_d_obj_h = obj_width >> 2;
            sb_d_obj_w = sb.last_sb_height << 2;
            g2d_program_sub_block_q((cyg_uint8*)dst, (cyg_uint8*)src, obj_width , sb.last_sb_height, 
                sb_d_obj_w, sb_d_obj_h, src_width, dst_width, rate);
        }
        else
        {
            g2d_program_sub_block_q((cyg_uint8*)dst, (cyg_uint8*)src, obj_width , sb.last_sb_height, 
            obj_width, sb.last_sb_height,src_width, dst_width, rate);
        }
        g2d_queue_operation(opr);
    }
    else if (sb.num_sub_blocks > 1) {
        int dst2 = (int)dst;
        if (bank_swap) {
            dst2 += self_max_blk_size;
        }
        if(rotat == 0 || rotat == 5)
        {
            g2d_program_sub_block_q((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width, 
                sb.sub_block_height, obj_width, sb.sub_block_height, src_width, dst_width, rate);
            for (i = 0; i < sb.num_sub_blocks - 1; i++) 
            {
                g2d_queue_operation(opr);   
                if (opr_cmd != G2D_OPR_MEMSET) 
                {
                    src += sb.next_sb_offset_src;
                    G2D_cmdq_add(G2D_CMDQ_ID_BB_SRC_START, obj_adr((cyg_uint32)src));
                }
                dst2 += sb.next_sb_offset_dst;
                if (bank_swap) 
                {
                    if ((int)src & self_max_blk_size) // 4n + 2
                        dst2 -= self_max_blk_size;
                    else // 4n
                        dst2 += self_max_blk_size;
                }
                G2D_cmdq_add(G2D_CMDQ_ID_BB_DST_START, obj_adr((cyg_uint32)dst2));
            }
            opr |= 0x1000;
            g2d_program_sub_block_q((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width, 
                sb.last_sb_height, obj_width, sb.last_sb_height, src_width, dst_width, rate);
        }
        else if(rotat == 2)
        {
            sb_d_obj_w = sb.sub_block_height << 2;
            sb_d_obj_h = obj_width >> 2;
            g2d_program_sub_block_q((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width, 
                sb.sub_block_height, sb_d_obj_w, sb_d_obj_h, src_width, dst_width, rate);
            for (i = 0; i < sb.num_sub_blocks - 1; i++) 
            {
                g2d_queue_operation(opr);    
                src += sb.next_sb_offset_src;
                G2D_cmdq_add(G2D_CMDQ_ID_BB_SRC_START, obj_adr((cyg_uint32)src));
                dst2 += sb_d_obj_w;
                if (bank_swap) 
                {
                    if ((int)src & self_max_blk_size) // 4n + 2
                        dst2 -= self_max_blk_size;
                    else // 4n
                        dst2 += self_max_blk_size;
                }
                G2D_cmdq_add(G2D_CMDQ_ID_BB_DST_START, obj_adr((cyg_uint32)dst2));
            }
            sb_d_obj_w = sb.last_sb_height << 2; 
            opr |= 0x1000;
            g2d_program_sub_block_q((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width,
                sb.last_sb_height, sb_d_obj_w, sb_d_obj_h, src_width, dst_width, rate);
        }
        else if(rotat == 3)
        {
            sb_d_obj_w = sb.sub_block_height << 2;
            sb_d_obj_h = obj_width >> 2;
            dst2 += (obj_height-sb.sub_block_height) << 2;
            g2d_program_sub_block_q((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width, 
                sb.sub_block_height, sb_d_obj_w, sb_d_obj_h, src_width, dst_width, rate);             
            for (i = 0; i < sb.num_sub_blocks - 1; i++) 
            {
                g2d_queue_operation(opr);    
                src += sb.next_sb_offset_src;
                G2D_cmdq_add(G2D_CMDQ_ID_BB_SRC_START, obj_adr((cyg_uint32)src));
                dst2 -= sb_d_obj_w;
                if (bank_swap) 
                {
                    if ((int)src & self_max_blk_size) // 4n + 2
                        dst2 -= self_max_blk_size;
                    else // 4n
                        dst2 += self_max_blk_size;
                }
                G2D_cmdq_add(G2D_CMDQ_ID_BB_DST_START, obj_adr((cyg_uint32)dst2));
            }
            sb_d_obj_w = sb.last_sb_height << 2; 
            dst2 += (sb.sub_block_height - sb.last_sb_height) << 2;
            opr |= 0x1000;
            g2d_program_sub_block_q((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width,
                sb.last_sb_height, sb_d_obj_w, sb_d_obj_h, src_width, dst_width, rate);
        }
        else if(rotat == 4 || rotat == 6)
        {
            dst2 += (obj_height - sb.sub_block_height) * dst_width;
            g2d_program_sub_block_q((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width, 
                sb.sub_block_height, obj_width, sb.sub_block_height, src_width, dst_width, rate);
            for (i = 0; i < sb.num_sub_blocks - 1; i++) 
            {
                g2d_queue_operation(opr);  
                src += sb.next_sb_offset_src;
                G2D_cmdq_add(G2D_CMDQ_ID_BB_SRC_START, obj_adr((cyg_uint32)src));
                dst2 -= sb.next_sb_offset_dst;
                if (bank_swap) 
                {
                    if ((int)src & self_max_blk_size) // 4n + 2
                        dst2 -= self_max_blk_size;
                    else // 4n
                        dst2 += self_max_blk_size;
                }
                G2D_cmdq_add(G2D_CMDQ_ID_BB_DST_START, obj_adr((cyg_uint32)dst2));
            }
            dst2 += (sb.sub_block_height - sb.last_sb_height) * dst_width;
            opr |= 0x1000;
            g2d_program_sub_block_q((cyg_uint8*)dst2, (cyg_uint8*)src, obj_width,
                sb.last_sb_height, obj_width, sb.last_sb_height, src_width, dst_width, rate);            
        }
        g2d_queue_operation(opr);   
    }
    G2D_cmdq_add(G2D_CMDQ_ID_IRQEN, en_irq_odmadone(0));
    /* Why disable cmd queue mode here? I want to make every G2D memory
       alteration routine independent of each other in terms of whether 
       the previous or next routine is using cmd queue or not. */

    while (((HAL_Value_Read_UINT32(G2D_status) >> 4) & 0x1ff) == 0) //wait till empty cmdq
        ;
    HAL_AND_UINT32(G2D_ctl, en_cmdq(0));
  
    return 0;
}
 
/* units are all in bytes */
/* the ratio must be 4 byte/pixel to 4 byte/pixel */
void G2D_alpha_blend(G2D_data *data)               
{
    HAL_WRITE_UINT32(G2D_alpha_mode,(updatedstalpha(data->alpha_data.updatedstalpha)|dst_usedefaultalpha(data->alpha_data.dst_en_def_alpha)|
        src_usedefaultalpha(data->alpha_data.src_en_def_alpha)|dst_defaultalpha(data->alpha_data.dst_def_alpha)|src_defaultalpha(data->alpha_data.src_def_alpha)));
    G2D_memalt(G2D_OPR_ALPBLN, data->dst, data->src, data->dmac_sel, data->alpha_data.mode, 0,
        1, 0, data->obj_width, data->obj_height, data->src_width, data->dst_width);
}

static int g2d_max_blk_size(G2D_BLK b)
{
    switch (b) {
    case G2D_BLK_NORMAL:
        return 4096;
    case G2D_BLK_BANK_SWAP:
        return 2048;
    case G2D_BLK_PALETTE:
        return 1024;
    default:
        return 4096;
    }
}
// units in byte
static void g2d_program_sub_block(cyg_uint8 *dst, cyg_uint8 *src, int s_obj_width, int s_obj_height,
                                  int d_obj_width, int d_obj_height, int src_width, int dst_width, int rate)
{
    int src_delx = 0, dst_delx = 0;
    if (rate == 1)
    {
        d_obj_width >>= 2;
        s_obj_width >>= 2; // byte to pixel
        src_width >>= 2;
        dst_width >>= 2;
    }
    else if (rate == 2)
    {
        dst_width >>= 2;
    }
    if (src) 
    {
        HAL_WRITE_UINT32(G2D_src_start, obj_adr((cyg_uint32)src));
        HAL_WRITE_UINT32(G2D_src_dxdy, (obj_dy(s_obj_height) | obj_dx(s_obj_width))); // in pixels
        if (rate == 1)
            src_delx = ((s_obj_width << 2) + ((cyg_uint32)src & 7) + 7) >> 3;
        else
        {
            src_delx = (s_obj_width + ((cyg_uint32)src & 7) + 7) >> 3;
        }     
        HAL_WRITE_UINT32(G2D_src_wid, (dma_dx8(src_delx) | gp_wid(src_width)));
        HAL_WRITE_UINT32(G2D_bb_src_txnr, tx_nr8(src_delx * s_obj_height));        
      }
      HAL_WRITE_UINT32(G2D_dst_start, obj_adr((cyg_uint32)dst));
      HAL_WRITE_UINT32(G2D_dst_dxdy, (b_obj_dy(d_obj_height) | obj_dx(d_obj_width)));

      if (rate == 1 || rate == 2)
          dst_delx = ((d_obj_width << 2) + ((cyg_uint32)dst & 7) + 7) >> 3; 
      else
      {
          dst_delx = (d_obj_width + ((cyg_uint32)dst & 7) + 7) >> 3;
      }
      HAL_WRITE_UINT32(G2D_dst_wid,( dma_dx8(dst_delx) | gp_wid(dst_width)));
      HAL_WRITE_UINT32(G2D_bb_dst_txnr, tx_nr8(dst_delx * d_obj_height));
}

// units in bytes
static void g2d_program_sub_block_q(cyg_uint8 *dst, cyg_uint8 *src, int s_obj_width, int s_obj_height,
                                    int d_obj_width, int d_obj_height, int src_width, int dst_width, int rate)
{
    if (rate == 1) {
        s_obj_width >>= 2; // byte to pixel
        d_obj_width >>= 2;
        src_width >>= 2;
        dst_width >>= 2;
    }
    else if (rate == 2)
    {
        dst_width >>= 2;
    }

    if (src) {
        G2D_cmdq_add(G2D_CMDQ_ID_BB_SRC_START, obj_adr((cyg_uint32)src));
        G2D_cmdq_add(G2D_CMDQ_ID_SRC_DXDY, (obj_dy(s_obj_height) | obj_dx(s_obj_width)));
        int src_delx;
        if (rate == 1)
            src_delx = ((s_obj_width << 2) + ((cyg_uint32)src & 7) + 7) >> 3;
        else
            src_delx = (s_obj_width + ((cyg_uint32)src & 7) + 7) >> 3;        
        G2D_cmdq_add(G2D_CMDQ_ID_SRC_WID, (dma_dx8(src_delx) | gp_wid(src_width)));
        G2D_cmdq_add(G2D_CMDQ_ID_BB_SRC_TXNR, tx_nr8(src_delx * s_obj_height));
    }
    G2D_cmdq_add(G2D_CMDQ_ID_BB_DST_START, obj_adr((cyg_uint32)dst));
    G2D_cmdq_add(G2D_CMDQ_ID_DST_DXDY, (b_obj_dy(d_obj_height) | obj_dx(d_obj_width)));
    int dst_delx;
    if (rate == 1 || rate == 2)
        dst_delx = ((d_obj_width << 2) + ((cyg_uint32)dst & 7) + 7) >> 3; 
    else
        dst_delx = (d_obj_width + ((cyg_uint32)dst & 7) + 7) >> 3; 
    G2D_cmdq_add(G2D_CMDQ_ID_DST_WID, (dma_dx8(dst_delx) | gp_wid(dst_width)));
    G2D_cmdq_add(G2D_CMDQ_ID_BB_DST_TXNR, tx_nr8(dst_delx * d_obj_height));
}

static void g2d_start_operation(int opr)
{ 
    HAL_WRITE_UINT32(G2D_irqen, en_irq_rg_rqcmd(1) | en_irq_odmadone(1));
    HAL_WRITE_UINT32(G2D_operation, opr);
    while ( (HAL_Value_Read_UINT32(G2D_status) & 0x8) == 0 ) { // polls odma done
    }
    HAL_WRITE_UINT32(G2D_status, irq_odmadone(0)); // clears odma done irq
    HAL_WRITE_UINT32(G2D_ctl, sw_rst(0));
}

static void g2d_queue_operation(int opr)
{
    int last_sb = (opr >> 12) & 1;
    if (last_sb) {
        //enable output dma done irq
        G2D_cmdq_add(G2D_CMDQ_ID_IRQEN, en_irq_rg_rqcmd(1) | en_irq_odmadone(1));
    }
    G2D_cmdq_add(G2D_CMDQ_ID_OPERATION, opr);
    if (last_sb) {
        while ( (HAL_Value_Read_UINT32(G2D_status) & 0x8) == 0 ) { 
        }
        //disable output dma done irq and clear it
        G2D_cmdq_add(G2D_CMDQ_ID_IRQEN, 0);
     HAL_WRITE_UINT32(G2D_status, irq_odmadone(0));
    }
}
/* units are in bytes */
static void dissect_sub_block(int obj_width, int obj_height, int src_width,
                              int dst_width, G2D_Subblock_Info *sb, int max_blk_size)
{
    int obj_size = obj_width * obj_height;
    sb->sub_block_height = max_blk_size / obj_width;
    int sub_block_size = sb->sub_block_height * obj_width;
    sb->next_sb_offset_src = sb->sub_block_height * src_width;
    sb->next_sb_offset_dst = sb->sub_block_height * dst_width;
    sb->last_sb_height = obj_height % sb->sub_block_height;
    if (sb->last_sb_height == 0)
        sb->last_sb_height = sb->sub_block_height;
    sb->num_sub_blocks = (obj_size - 1) / sub_block_size + 1; 

#ifdef __2d_debug_mode
    diag_printf("obj_width : %d\n",obj_width);
    diag_printf("obj_height : %d\n",obj_height);
    diag_printf("src_width : %d\n",src_width);
    diag_printf("dst_width: %d\n",dst_width);
    diag_printf("max_blk_size : %d\n",max_blk_size);
    diag_printf("sb->next_sb_offset_src : %d\n",sb->next_sb_offset_src);
    diag_printf("sb->next_sb_offset_dst : %d\n",sb->next_sb_offset_dst);
    diag_printf("sb->last_sb_height : %d\n",sb->last_sb_height);
    diag_printf("num_sub_blocks : %d\n",sb->num_sub_blocks);  
#endif
}

static cyg_uint32 magnify_ratio(cyg_uint32 n, cyg_uint32 m, cyg_uint32 *out_n, cyg_uint32 *out_m)
{
    cyg_uint32 add_to_i = 0;
    int flag = 1;
    // handle corner cases first
    if (n == 0 || m == 0) {
        flag = 0;
    }
    else if (n == m) {
        flag = 0;
        add_to_i = 1;
    }
    else if (n > m) {
        if ((n % m) == 0) {
            flag = 0;
            add_to_i = n / m;
        }
        else {
            while (n > m) {
                n -= m;
                add_to_i++;
            }
        }
    }
    if (flag == 0) {
        *out_n = *out_m = 0;
        return add_to_i;
    }
    // normal case: 0 < N < M <= 256
    cyg_uint32 q = 256 / m;
    cyg_uint32 r = 256 % m;
    *out_n = n * q;
    if (r == 0) {
        *out_m = 0; /* 256 */
    }
    else {
        *out_m = m * q;
    }    
    return add_to_i;
}
static void g2d_dmac_clear(void)
{
    HAL_WRITE_UINT32(dmac_init_addr_g2d_dst_out, 1);  //ch 9
    HAL_WRITE_UINT32(dmac_init_mode_g2d_dst_out, G2D_DMAC_INIT_MODE);
    HAL_WRITE_UINT32(dmac_init_addr_g2d_src_in, 1);  //ch 8
    HAL_WRITE_UINT32(dmac_init_mode_g2d_src_in, G2D_DMAC_INIT_MODE);
    HAL_WRITE_UINT32(dmac_init_addr_g2d_dst_in, 1);  //ch 10
    HAL_WRITE_UINT32(dmac_init_mode_g2d_dst_in, G2D_DMAC_INIT_MODE); 
}
bool Q_empty(void)
{
    return (HAL_Value_Read_UINT32(G2D_status)& 0x1);
}
void lag_g2d(int x) // x = 0.01 sec
{
    int j = 0;
    cyg_uint32 i = 0;
    for(j = 0; j < x; j++)
    {
        for(i = 0; i < 40000; i++)
        {
            asm("NOP");
        }
    }
}
void read_2d_register(void)
{
    diag_printf("0x%x  : %08x\n",G2D_src_start,HAL_Value_Read_UINT32(G2D_src_start));
    diag_printf("0x%x  : %08x\n",G2D_src_dxdy,HAL_Value_Read_UINT32(G2D_src_dxdy));
    diag_printf("0x%x  : %08x\n",G2D_src_wid,HAL_Value_Read_UINT32(G2D_src_wid));
    diag_printf("0x%x  : %08x\n",G2D_bb_src_txnr,HAL_Value_Read_UINT32(G2D_bb_src_txnr));
    diag_printf("0x%x  : %08x\n",G2D_dst_start,HAL_Value_Read_UINT32(G2D_dst_start));
    diag_printf("0x%x  : %08x\n",G2D_dst_dxdy,HAL_Value_Read_UINT32(G2D_dst_dxdy));
    diag_printf("0x%x  : %08x\n",G2D_dst_wid,HAL_Value_Read_UINT32(G2D_dst_wid));
    diag_printf("0x%x  : %08x\n",G2D_bb_dst_txnr,HAL_Value_Read_UINT32(G2D_bb_dst_txnr));
    diag_printf("0x%x  : %08x\n",G2D_ctl,HAL_Value_Read_UINT32(G2D_ctl));
    diag_printf("0x%x  : %08x\n",G2D_operation,HAL_Value_Read_UINT32(G2D_operation)); 
    diag_printf("0x%x  : %08x\n",G2D_sc_h_ratio,HAL_Value_Read_UINT32(G2D_sc_h_ratio));       
    diag_printf("0x%x  : %08x\n",G2D_sc_v_ratio,HAL_Value_Read_UINT32(G2D_sc_v_ratio));
}

void g2d_memory_sync(cyg_uint8 *dst, cyg_uint16 obj_width, cyg_uint16 obj_height,
                     cyg_uint16 dst_pitch)
{
    int i = 0;
    cyg_uint32 data_addr   = (cyg_uint32)dst; 
    cyg_uint16 data_length = obj_width << 2; // byteperpixel
    cyg_uint16 data_pitch  = dst_pitch << 2;
    for(i = 0; i < obj_height; i++)
    {
        R16_CACHE_SYNC(data_addr, data_length);
        data_addr += data_pitch;
    }
}

