/*
Digital Panel
//======================================================
HAL_WRITE_UINT32(0x90005300, 0x00D7007F);
HAL_WRITE_UINT32(0x90005304, 0x041F03F7);
HAL_WRITE_UINT32(0x90005308, 0x00240003);
HAL_WRITE_UINT32(0x9000530C, 0x020C0204);
HAL_WRITE_UINT32(0x90005310, 0x320F0320);
HAL_WRITE_UINT32(0x90005314, 0x40300000);
HAL_WRITE_UINT32(0x90005318, 0x40477000);
HAL_WRITE_UINT32(0x9000531C, 0x00FF0000);
HAL_WRITE_UINT32(0x90005324, 0x00C300C0);
HAL_WRITE_UINT32(0x90005328, 0x03FE0200);
HAL_WRITE_UINT32(0x9000532C, 0x02100001);
HAL_WRITE_UINT32(0x90005330, 0x00260024);
HAL_WRITE_UINT32(0x90005334, 0x03E003A9);
HAL_WRITE_UINT32(0x90005338, 0x00400060);
HAL_WRITE_UINT32(0x9000533C, 0x004020E0);  //frame 0
HAL_WRITE_UINT32(0x9000533C, 0x004020E8);  //frame 1	
HAL_WRITE_UINT32(0x90005340, 0x00000001);
//======================================================
Analog Panel
HAL_WRITE_UINT32(0x90005300, 0x001D0004);
HAL_WRITE_UINT32(0x90005304, 0x023C01FD);
HAL_WRITE_UINT32(0x90005308, 0x00060003);
HAL_WRITE_UINT32(0x9000530C, 0x00FF00F0);  //FF --> FE for real chip jiunhau 980219
HAL_WRITE_UINT32(0x90005310, 0x1E0751E0);
HAL_WRITE_UINT32(0x90005314, 0x40300000);
HAL_WRITE_UINT32(0x90005318, 0x40477000);
HAL_WRITE_UINT32(0x9000531C, 0x00FF0000);
HAL_WRITE_UINT32(0x90005324, 0x0021001E);
HAL_WRITE_UINT32(0x90005328, 0x021C01FE);
HAL_WRITE_UINT32(0x9000532C, 0x010E8001);
HAL_WRITE_UINT32(0x90005330, 0x00050003);
HAL_WRITE_UINT32(0x90005334, 0x01FE01C7);
HAL_WRITE_UINT32(0x90005338, 0x00400060);
HAL_WRITE_UINT32(0x9000533C, 0x004121E2);
HAL_WRITE_UINT32(0x90005340, 0x00000001);
//======================================================
Analog Panel(FOR REAL CHIP)
HAL_WRITE_UINT32(0x90005300, 0x001D0004);
HAL_WRITE_UINT32(0x90005304, 0x027B01FD);
HAL_WRITE_UINT32(0x90005308, 0x00060003);
HAL_WRITE_UINT32(0x9000530C, 0x010500f0);
HAL_WRITE_UINT32(0x90005310, 0x1E0751E0);
HAL_WRITE_UINT32(0x90005314, 0x40300000);
HAL_WRITE_UINT32(0x90005318, 0x40477000);
HAL_WRITE_UINT32(0x9000531C, 0x00FF0000);
HAL_WRITE_UINT32(0x90005324, 0x0021001E);
HAL_WRITE_UINT32(0x90005328, 0x021C01FE);
HAL_WRITE_UINT32(0x9000532C, 0x010E0010);
HAL_WRITE_UINT32(0x90005330, 0x00050003);
HAL_WRITE_UINT32(0x90005334, 0x01FE01C7);
HAL_WRITE_UINT32(0x90005338, 0x00400060);
HAL_WRITE_UINT32(0x9000533C, 0x004121E2);
HAL_WRITE_UINT32(0x90005340, 0x00000001);
//======================================================
Analog Panel(FOR AUO analog)
HAL_WRITE_UINT32(0x90005300, 0x00040004);
HAL_WRITE_UINT32(0x90005304, 0x027A01E4);
HAL_WRITE_UINT32(0x90005308, 0x00050002);
HAL_WRITE_UINT32(0x9000530C, 0x00FE00EF);
HAL_WRITE_UINT32(0x90005310, 0x1E0751E0);
HAL_WRITE_UINT32(0x90005314, 0x40300000);
HAL_WRITE_UINT32(0x90005318, 0x40477000);
HAL_WRITE_UINT32(0x9000531C, 0x00FF0000);
HAL_WRITE_UINT32(0x90005324, 0x00080005);
HAL_WRITE_UINT32(0x90005328, 0x020301E5);
HAL_WRITE_UINT32(0x9000532C, 0x013D8001);
HAL_WRITE_UINT32(0x90005330, 0x00070005);
HAL_WRITE_UINT32(0x90005334, 0x01E501AE);
HAL_WRITE_UINT32(0x90005338, 0x00400060);
HAL_WRITE_UINT32(0x9000533C, 0x00C121E2);
HAL_WRITE_UINT32(0x90005340, 0x00000001);

*/
#include <pkgconf/hal.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_io.h>       // IO macros
#include <cyg/hal/hal_if.h>       // Virtual vector support
#include <cyg/hal/hal_arch.h>     // Register state info
#include <cyg/hal/hal_intr.h>     // HAL interrupt macros

#include <stdio.h>  // sscanf
#include <stdlib.h>
#include <math.h>

#include <cyg/io/display/crt/Crt_io.h>
#include <cyg/devs/display/crt/Crt_dev.h>

#ifdef  CYGPKG_CRT_PANEL_DIGITAL
#define DISPLAY_HEIGHT 480
#define DISPLAY_WIDTH  800
#elif defined(CYGPKG_CRT_PANEL_ANALOG)
#define DISPLAY_HEIGHT 234
#define DISPLAY_WIDTH  480
#endif
#define bpp1 32
#ifdef __IRQ_test
static cyg_interrupt CRT_inter_data;
static cyg_handle_t  CRT_inter_handle;
#endif
static cyg_devio_table_t CRT_devio;

void *lcd_framebuffer1 = NULL, *lcd_framebuffer2 = NULL; 
CRT_mode *CRT_mode_set = NULL;

static bool CRT_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo CRT_lookup(struct cyg_devtab_entry **tab, 
                            struct cyg_devtab_entry *sub_tab,
                            const char *name);
static Cyg_ErrNo CRT_set_config(cyg_io_handle_t handle, cyg_uint32 key, void *buf, cyg_uint32 *len);
static Cyg_ErrNo CRT_read(cyg_io_handle_t handle, void *buf, cyg_uint32 *len);
//void CRT_srtting_for_AUO_analog_panel();


extern void *malloc_align( size_t /* size */, size_t /* alignment */);

static DEVIO_TABLE(CRT_devio,
                   NULL,
                   CRT_read,
                   NULL,
                   NULL,
                   CRT_set_config
                   );

static DEVTAB_ENTRY(CRT_devtab_entry, 
             "/dev/display/crt",
             0,
             &CRT_devio, 
             CRT_init, 
             CRT_lookup,      // Execute this when device is being looked up
             0);

bool CRT_isr(cyg_vector_t vect)
{
	return true;
}

void CRT_dsr(cyg_vector_t vect)
{
}
static bool CRT_init(struct cyg_devtab_entry *tab)
{
    lcd_framebuffer1 = malloc_align(sizeof(cyg_uint32) * DISPLAY_HEIGHT * DISPLAY_WIDTH, 1024);
    lcd_framebuffer2 = malloc_align(sizeof(cyg_uint32) * DISPLAY_HEIGHT * DISPLAY_WIDTH, 1024);
	HAL_WRITE_UINT32(dmac_init_addr_crt, 1);  //ch 0
	HAL_WRITE_UINT32(dmac_init_mode_crt, CRT_DMAC_INIT_MODE);   
#ifdef __IRQ_test
	cyg_drv_interrupt_mask(CRT_IRQ_NUM);
	cyg_drv_interrupt_create(CRT_IRQ_NUM,
                             99,        // priority
                             0,         // data
                             &CRT_isr,                             
                             &CRT_dsr,
                             &CRT_inter_handle,
                             &CRT_inter_data);
	cyg_drv_interrupt_attach(CRT_inter_handle);
	cyg_drv_interrupt_unmask(CRT_IRQ_NUM);
#endif	
  	CRT_lcd_set d1 = {0};
  	CRT_hv_table hor = {0}, ver = {0};
  	lcd_info *lcd_qq = NULL;
  	lcd_getinfo(lcd_qq);
#ifdef CYGPKG_CRT_PANEL_DIGITAL
  	/*Hor*/
	hor.size = lcd_qq->width;
	hor.actveregion = hor.size;
	hor.backporch = 88;
	hor.frontporch = 0;
	hor.lb_border = 0;
	hor.sycend = 1056;
	hor.sync = 128;
	/*Ver*/
	ver.size = lcd_qq->height;
	ver.actveregion = ver.size;
	ver.backporch = 33;
	ver.frontporch = 33;
	ver.lb_border = 0;
	ver.sycend = 525;
	ver.sync = 4;
	/*calculate*/
	d1.hor_syncend = hor.sync - 1;
	d1.hor_blankend = hor.backporch + hor.lb_border + d1.hor_syncend;
	d1.hor_activeend = hor.size + d1.hor_blankend;
	d1.hor_end = hor.sycend - 1;
	d1.ver_syncend = ver.sync - 1;
	d1.ver_blankend = ver.backporch + ver.lb_border + d1.ver_syncend;
	d1.ver_activeend = ver.size + d1.ver_blankend;
	d1.ver_end = ver.sycend - 1;
	/*original*/
	HAL_WRITE_UINT32(horsyncend, (CRT_horsyncend(d1.hor_syncend) | CRT_horblankend(d1.hor_blankend)));
	HAL_WRITE_UINT32(horactiveend, (CRT_horactiveend(d1.hor_activeend) | CRT_horend(d1.hor_end)));
	HAL_WRITE_UINT32(versyncend, (CRT_versyncend(d1.ver_syncend) | CRT_verblankend(d1.ver_blankend)));
	HAL_WRITE_UINT32(veractiveend, (CRT_veractiveend(d1.ver_activeend) | CRT_verend(d1.ver_end)));
	HAL_WRITE_UINT32(linepitch, (CRT_horsize(lcd_qq->width) | CRT_versize(lcd_qq->height) | CRT_linepitch(400)));
	/*start_end*/
	HAL_WRITE_UINT32(STH, (CRT_start(192) | CRT_end(195)));
	HAL_WRITE_UINT32(OEH, (CRT_start(512) | CRT_end(1022)));
	HAL_WRITE_UINT32(CKV, (CRT_start(1)   | CRT_end(528)));
	HAL_WRITE_UINT32(STV, (CRT_STVstart(36) | CRT_STVend(38)));
	HAL_WRITE_UINT32(OEV, (CRT_start(937) | CRT_end(992)));
#endif
//=========================================================================================
#ifdef CYGPKG_CRT_PANEL_ANALOG
  	/*original*/
  	HAL_WRITE_UINT32(horsyncend  , (CRT_horsyncend(4)	  | CRT_horblankend(29)));
    #ifndef CYG_HAL_CHIP_PLATFORM_REALCHIP
    HAL_WRITE_UINT32(horactiveend, (CRT_horactiveend(509) | CRT_horend(572)));//FPGA
    #else
    HAL_WRITE_UINT32(horactiveend, 0x27b01fd);
    #endif
    HAL_WRITE_UINT32(versyncend  , (CRT_versyncend(3)	  | CRT_verblankend(6)));
    #ifndef CYG_HAL_CHIP_PLATFORM_REALCHIP
    HAL_WRITE_UINT32(veractiveend, (CRT_veractiveend(240) | CRT_verend(254))); 
    // 255 --> 254  980219 jiunhau 
    #else
    HAL_WRITE_UINT32(veractiveend, 0x10500f0);
    #endif    
    HAL_WRITE_UINT32(linepitch   , (CRT_horsize(480)      | CRT_versize(234) | CRT_linepitch(240)));

    /*start_end*/
  	HAL_WRITE_UINT32(STH, (CRT_start(30)  | CRT_end(33)));
  	HAL_WRITE_UINT32(OEH, (CRT_start(510) | CRT_end(540)));
    #ifndef CYG_HAL_CHIP_PLATFORM_REALCHIP
    HAL_WRITE_UINT32(CKV, (CRT_start(1)	  | CRT_end(270)));//FPGA
    #else
    HAL_WRITE_UINT32(CKV, 0x10e0010);
    #endif
    HAL_WRITE_UINT32(STV, (CRT_STVstart(3)| CRT_STVend(5)));
	HAL_WRITE_UINT32(OEV, (CRT_start(455) | CRT_end(510)));
    //CRT_srtting_for_AUO_analog_panel();
#endif
    CRT_mode_set->dacpd = 0;
    CRT_mode_set->usebgcolor = 0;
    CRT_mode_set->engamma = 0;
    CRT_mode_set->test_mode = 0;
  	/*polarity*/
    CRT_mode_set_fun(CRT_mode_set);
	/*FIFO*/
    HAL_WRITE_UINT32(fifo_threshold, (CRT_fifothhigh(120) | CRT_fifothlow(96)));
	CRT_frame_set((cyg_uint32)lcd_framebuffer1, (cyg_uint32)lcd_framebuffer2, 0);
	CRT_go(1);
#ifdef CYGPKG_CRT_SETTING
    CRT_read_register();
#endif
	return true;
}

static Cyg_ErrNo CRT_read(cyg_io_handle_t handle, void *buf, cyg_uint32 *len)
{
	lcd_info *getbuf = (lcd_info *)buf;
  	getbuf->height = DISPLAY_HEIGHT;
	getbuf->width = DISPLAY_WIDTH;
	getbuf->bpp = bpp1;
	getbuf->rlen = DISPLAY_WIDTH << 2;
	getbuf->fb1 = (void *)lcd_framebuffer1;
	getbuf->fb2 = (void *)lcd_framebuffer2;
	return ENOERR;
}

static Cyg_ErrNo CRT_set_config(cyg_io_handle_t handle, cyg_uint32 key, void *buf, cyg_uint32 *len)
{
	CRT_option *getbuf = (CRT_option *)buf;
	switch (key) {
        case 1://gamma use
			CRT_gamma_use(getbuf->on);
			break;
		case 2://gamma set
			CRT_init_gamma_table(getbuf->ga);
			break;
		case 3://frame set
			CRT_frame_set((cyg_uint32)lcd_framebuffer1, (cyg_uint32)lcd_framebuffer2, getbuf->choice);
			break;
		case 4://background color
			CRT_set_bg(getbuf->CRT_r, getbuf->CRT_g, getbuf->CRT_b, getbuf->on);
			break;
		case 5:
			CRT_test(getbuf->on);
			break;
		case 6:
			CRT_power_down(getbuf->on);
			break;	
		default:
			break;
		}
	return ENOERR;
}
static Cyg_ErrNo CRT_lookup(struct cyg_devtab_entry **tab, 
                            struct cyg_devtab_entry *sub_tab,
                            const char *name)
{
	return ENOERR;
}
void CRT_init_gamma_table(float g)
{
    //v code 971106 will write half --> 32  97-11-17
	int i = 0;
	cyg_uint8 tbl[256] = {0};
	cyg_uint32 result = 0;
    //cyg_uint32 a = 0; 
	for(i = 0; i < 256; i++) 
	{
		tbl[i] = (cyg_uint8)(pow(i / 255.0, g) * 255.0 );     	
    }
	for(i = 0; i < 256; i+=4)
	{
	    //a = 0;
		result = (tbl[i+3] << 24) | (tbl[i+2] << 16) | (tbl[i+1] << 8) | tbl[i];
		HAL_WRITE_UINT32(gamma, CRT_gammadata(result));

        //HAL_READ_UINT32(gamma, a);
        //diag_printf("count : %d | write : %08x | read : %08x\n", i>>2, result, a);
    }
}
void CRT_gamma_use(bool on)
{
    CRT_mode_set->engamma = on;
    CRT_mode_set_fun(CRT_mode_set);
}
void CRT_test(bool on)
{
    CRT_mode_set->test_mode = on;
    CRT_mode_set_fun(CRT_mode_set);
}
void CRT_go(bool on)
{
	HAL_WRITE_UINT32(sgcrtdisplay, CRT_enable(on));
}
void CRT_power_down(bool on)
{
    CRT_mode_set->dacpd = on;
    CRT_mode_set_fun(CRT_mode_set);
}
void CRT_set_bg(cyg_uint8 CRT_r,cyg_uint8 CRT_g,cyg_uint8 CRT_b,bool on)
{
	HAL_WRITE_UINT32(background,CRT_background((((CRT_r & 0xff) << 16) | ((CRT_g & 0xff) << 8) | (CRT_b & 0xff))));
    CRT_mode_set->usebgcolor = on;
    CRT_mode_set_fun(CRT_mode_set);
}
void CRT_frame_set(cyg_uint32 addr1,cyg_uint32 addr2,bool choice)
{
	HAL_WRITE_UINT32(address0, CRT_startaddress(addr1));
	HAL_WRITE_UINT32(address1, CRT_startaddress(addr2));
    CRT_mode_set->addressflip = choice;
    CRT_mode_set_fun(CRT_mode_set);
}
void CRT_mode_set_fun(CRT_mode *a)
{
#ifdef CYGPKG_CRT_PANEL_DIGITAL
    HAL_WRITE_UINT32(sgcrtpolarity, CRT_hsyncpolarity(1) | CRT_vsyncpolarity(1)|CRT_dataendian(1)|
        CRT_cphmode(1)|CRT_seldaccph(1)|CRT_endither(1)|CRT_dgcrtstartsel(2)|CRT_sgcrtstartsel(2)|
        CRT_addressflip(a->addressflip)|CRT_engamma(a->engamma)|CRT_dacpd(a->dacpd) |
        CRT_testmode(a->test_mode)|CRT_usebgcolor(a->usebgcolor));
#endif
#ifdef CYGPKG_CRT_PANEL_ANALOG //FPGA : CRT_sgcrtstartsel=2
    #ifndef CYG_HAL_CHIP_PLATFORM_REALCHIP
    HAL_WRITE_UINT32(sgcrtpolarity, CRT_hsyncpolarity(1)| CRT_vsyncpolarity(1)|CRT_dataendian(1)|CRT_sgcrtstartsel(0)|
        CRT_cphmode(1)|CRT_seldaccph(1)|CRT_endither(0)| CRT_dgcrtstartsel(0)|CRT_oehpolarity(1)|CRT_datainvert(1)|
        CRT_addressflip(a->addressflip)  |CRT_engamma(a->engamma)|CRT_dacpd(a->dacpd) |
        CRT_testmode(a->test_mode)|CRT_usebgcolor(a->usebgcolor));    
    #else
    HAL_WRITE_UINT32(sgcrtpolarity, CRT_hsyncpolarity(1)| CRT_vsyncpolarity(1)|CRT_dataendian(1)|CRT_sgcrtstartsel(0)|
        CRT_cphmode(3)|CRT_seldaccph(1)|CRT_endither(0)| CRT_dgcrtstartsel(0)|CRT_oehpolarity(1)|CRT_datainvert(1)|
        CRT_dacphase(1)|CRT_addressflip(a->addressflip)  |CRT_engamma(a->engamma)|CRT_dacpd(a->dacpd) |
        CRT_testmode(a->test_mode)|CRT_usebgcolor(a->usebgcolor));
    
    #endif
#endif
}

void CRT_read_register(void)
{
	diag_printf("0x%x  : %08x\n",horsyncend,HAL_Value_Read_UINT32(horsyncend));
	diag_printf("0x%x  : %08x\n",horactiveend,HAL_Value_Read_UINT32(horactiveend));
	diag_printf("0x%x  : %08x\n",versyncend,HAL_Value_Read_UINT32(versyncend));
	diag_printf("0x%x  : %08x\n",veractiveend,HAL_Value_Read_UINT32(veractiveend));
	diag_printf("0x%x  : %08x\n",linepitch,HAL_Value_Read_UINT32(linepitch));
	diag_printf("0x%x  : %08x\n",STH,HAL_Value_Read_UINT32(STH));
	diag_printf("0x%x  : %08x\n",OEH,HAL_Value_Read_UINT32(OEH));
	diag_printf("0x%x  : %08x\n",CKV,HAL_Value_Read_UINT32(CKV));
	diag_printf("0x%x  : %08x\n",STV,HAL_Value_Read_UINT32(STV));
	diag_printf("0x%x  : %08x\n",OEV,HAL_Value_Read_UINT32(OEV));
	diag_printf("0x%x  : %08x\n",sgcrtpolarity,HAL_Value_Read_UINT32(sgcrtpolarity));
	diag_printf("0x%x  : %08x\n",fifo_threshold,HAL_Value_Read_UINT32(fifo_threshold));
	diag_printf("0x%x  : %08x\n",sgcrtdisplay,HAL_Value_Read_UINT32(sgcrtdisplay));
	diag_printf("0x%x  : %08x\n",sgcrtststus,HAL_Value_Read_UINT32(sgcrtststus));	
}

void CRT_srtting_for_AUO_analog_panel(void)
{
    HAL_WRITE_UINT32(0x90005300, 0x00040004);
    HAL_WRITE_UINT32(0x90005304, 0x027A01E4);
    HAL_WRITE_UINT32(0x90005308, 0x00050002);
    HAL_WRITE_UINT32(0x9000530C, 0x00FE00EF);
    HAL_WRITE_UINT32(0x90005310, 0x1E0751E0);
    HAL_WRITE_UINT32(0x90005324, 0x00080005);
    HAL_WRITE_UINT32(0x90005328, 0x020301E5);
    HAL_WRITE_UINT32(0x9000532C, 0x013D8001);   
    HAL_WRITE_UINT32(0x90005330, 0x00070005);
    HAL_WRITE_UINT32(0x90005334, 0x01E501AE);
    HAL_WRITE_UINT32(0x9000533C, 0x00C121E2);
}

