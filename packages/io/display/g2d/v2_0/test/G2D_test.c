#include <cyg/kernel/kapi.h>	// Kernel API.
#include <cyg/infra/diag.h>		// For diagnostic printing.
#include "G2D_io.h"
#include  <time.h>



void cyg_user_start( void )
{
	cyg_uint8 *s,*d;
	int o_w,o_h,s_w,d_w;
	int patterm;
	cyg_uint32 v_i,v_n,v_m,h_i,h_m,h_n;
	
#if 0
int g2d_memcpy(d,s, cyg_uint8 dmac_sel,o_w,o_h,s_w,d_w);

int g2d_memset(d,patterm, cyg_uint8 dmac_sel,o_w,o_h,d_w);

void g2d_alpha_blend(d,s, cyg_uint8 dmac_sel, ALPBLN_MODE mode,
                     o_w,o_h,s_w,d_w,bool f_dst_ab,
                     bool dst_ini_ab,cyg_uint8 dst_def_alpha,bool src_ini_ab,cyg_uint8 src_def_alpha);
int g2d_memmix(d,s, cyg_uint8 dmac_sel, int ropf,
               patterm,o_w,o_h,s_w,d_w);
int g2d_memscl(int dst_frmb, int src_frmb, cyg_uint8 dmac_sel, 
               cyg_uint32 src_x, cyg_uint32 src_y, cyg_uint32 dst_x, cyg_uint32 dst_y,
               v_i,v_n,v_m,h_i,h_n,h_m,o_w,o_h,s_w,d_w);
int g2d_memmap(d,s, cyg_uint8 dmac_sel, int obj_width, 
               int obj_height, int src_width, int dst_width,
               cyg_uint32 *palette_table,cyg_uint8 pal_entry);
int g2d_rotation(d,s, cyg_uint8 dmac_sel,int rate,   
               int rotat,o_w,o_h,s_w,d_w);

void g2d_flip_setting(cyg_uint32 addr,0,true);

void g2d_bilinear(s,int s_w,int s_h,d,float h_sca,float v_sca);
#endif
	g2d_dev_test();
	/*bitblt               22*/
	/*alpha blending 10*/
	/*rotation            7*/
	/*scaling               */
	/*palette               */
	/*flip exchange      */
	/*memcpy             */
	/*memset              */
}
