#ifndef _Crt_dev_H_
#define _Crt_dev_H_

#include <pkgconf/system.h>
#include <cyg/kernel/kapi.h>        //cyg_vector
#include <cyg/io/devtab.h>
#include <pkgconf/devs_crt.h>

//DMAC
#define CRT_DMAC_INIT_MODE ((1 << 20) | (1 << 9) | 1)
//Interrupt enable
#define CRT_IRQ_NUM     11

//DMAC
#ifdef CYG_HAL_CHIP_VERSION_910A
#define dmac_init_mode_crt      0x90001800 
#define dmac_init_addr_crt      0x90001804
#elif defined(CYG_HAL_CHIP_VERSION_910B)
#define dmac_init_mode_crt      0x90002800  //V vode 20081106
#define dmac_init_addr_crt      0x90002804
#endif

#define dmac_cur_mode_crt       0x90001808 
#define dmac_cur_addr_crt       0x9000180c

//CRT_register macro
#define CRT_horsyncend(x)       (x & 0x7FF)
#define CRT_horblankend(x)      ((x & 0x7FF) << 16 )
#define CRT_horactiveend(x)     (x & 0x7FF)
#define CRT_horend(x)           ((x & 0x7FF) << 16 )
#define CRT_versyncend(x)       (x & 0x3FF)
#define CRT_verblankend(x)      ((x & 0x3FF) << 16 )
#define CRT_veractiveend(x)     (x & 0x3FF)
#define CRT_verend(x)           ((x & 0x3FF) << 16 )
#define CRT_horsize(x)          (x & 0x7FF)
#define CRT_versize(x)          ((x & 0x3FF) << 11 )
#define CRT_linepitch(x)        ((x & 0x7FF) <<21 )
#define CRT_startaddress(x)    	(x & 0xFFFFFFFF)
#define CRT_background(x)       (x & 0xFFFFFF)
#define CRT_gammadata(x)        (x & 0xFFFFFFFF)
#define CRT_start(x)			(x & 0x7FF)
#define CRT_end(x)				((x & 0x7FF) << 16 )
#define CRT_STVstart(x)         (x & 0x3FF)
#define CRT_STVend(x)           ((x & 0x3FF) << 16 )
#define CRT_fifothhigh(x)       (x & 0xFF)
#define CRT_fifothlow(x)        ((x & 0xFF) << 16)
#define CRT_outmode(x)          (x & 0x1)
#define CRT_datainvert(x)		((x & 0x1) << 1)
#define CRT_engamma(x)          ((x & 0x1) << 2)
#define CRT_addressflip(x)      ((x & 0x1) << 3)
#define CRT_endither(x)         ((x & 0x1) << 4)
#define CRT_hsyncpolarity(x)    ((x & 0x1) << 5)
#define CRT_vsyncpolarity(x)    ((x & 0x1) << 6)
#define CRT_dataendian(x)       ((x & 0x3) << 7)
#define CRT_oddcolormode(x)     ((x & 0x3) << 9)
#define CRT_evencolormode(x)    ((x & 0x3) << 11)
#define CRT_cphmode(x)          ((x & 0x7) << 13)
#define CRT_oehpolarity(x)      ((x & 0x1) << 16)
#define CRT_sthpolarity(x)      ((x & 0x1) << 17)
#define CRT_oevpolarity(x)      ((x & 0x1) << 18)
#define CRT_stvpolarity(x)      ((x & 0x1) << 19)
#define CRT_ckvpolarity(x)      ((x & 0x1) << 20)
#define CRT_testmode(x)         ((x & 0x1) << 21)
#define CRT_seldaccph(x)        ((x & 0x1) << 22)
#define CRT_dacphase(x)         ((x & 0x1) << 23)
#define CRT_dacpd(x)            ((x & 0x1) << 24)
#define CRT_selserialout(x)     ((x & 0x1) << 25)
#define CRT_usebgcolor(x)       ((x & 0x1) << 26)
#define CRT_dgcrtstartsel(x)    ((x & 0x3) << 27)  //default 0x2
#define CRT_datainvert_0(x)     ((x & 0x1) << 29)
#define CRT_sgcrtstartsel(x)    ((x & 0x3) << 30)

#define CRT_enable(x)           (x & 0x1)
#define CRT_usecomdq(x)         ((x & 0x1) << 1 )

//CRT Register Defination
#define horsyncend     			0x90005300
#define horactiveend   			0x90005304
#define versyncend     			0x90005308
#define veractiveend   			0x9000530C
#define linepitch      			0x90005310
#define address0       			0x90005314
#define address1       			0x90005318
#define background     			0x9000531C
#define gamma          			0x90005320
#define STH            			0x90005324
#define OEH            			0x90005328
#define CKV            			0x9000532C
#define STV            			0x90005330
#define OEV            			0x90005334
#define fifo_threshold 			0x90005338
#define sgcrtpolarity  			0x9000533C
#define sgcrtdisplay   			0x90005340
#define sgcrtststus    			0x90005344

typedef struct{
	int hor_syncend;
	int hor_blankend;
	int hor_activeend;
	int hor_end;
	int ver_syncend;
	int ver_blankend;
	int ver_activeend;
	int ver_end;
	int line_pitch;
}CRT_lcd_set;

typedef struct{
	int sync;
	int backporch;
	int lb_border;
	int sycend;
	int actveregion;
	int rt_border;
	int frontporch;
	int size;
}CRT_hv_table;

typedef struct{
    bool engamma;
    bool usebgcolor;
    bool dacpd;
    cyg_uint8 test_mode;
    cyg_uint8 addressflip;
}CRT_mode;

#define HAL_Value_Read_UINT32(_reg_)  *((volatile CYG_WORD32 *)(_reg_))

static bool CRT_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo CRT_set_config(cyg_io_handle_t handle, cyg_uint32 key, void *buf, cyg_uint32 *len);
static Cyg_ErrNo CRT_read(cyg_io_handle_t handle, void *buf, cyg_uint32 *len);
bool CRT_isr(cyg_vector_t vect);
void CRT_dsr(cyg_vector_t vect);

/*
   \brief CRT setting background color
   \param : R G B
   \param : on
*/
void CRT_set_bg(cyg_uint8 CRT_r, cyg_uint8 CRT_g, cyg_uint8 CRT_b, bool on);
/*
   \brief let CRT get the change of option automatically. 
   \param :on 
*/
void CRT_go(bool on);
/*
   \brief let CRT  run the save mode.
   \param :on 
*/
void CRT_power_down(bool on);
/*
   \brief let CRT run the test mode.
   \param :on 
*/
void CRT_test(bool on);
/*
   \brief setting the start address of frame buffer and which one to use.
   \param  addr1
   \param  addr2
   \param  choose  addr1 or addr2?
*/
void CRT_frame_set(cyg_uint32 addr1, cyg_uint32 addr2, bool choice);
/*
   \brief let CRT display with the gamma.
   \param :on 
*/
void CRT_gamma_use(bool on);
/*
   \brief initial the gamma table
   \param :g  Gamma
*/
void CRT_init_gamma_table(float g);
/*
    \brief use to check the display is end
*/
void CRT_read_register(void);
void CRT_mode_set_fun(CRT_mode *a);
#endif
