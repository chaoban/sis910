#ifndef _otg_H_
#define _otg_H_

//#include <cyg/infra/cyg_type.h>	// base types
#include <cyg/io/usb.h>
#include <cyg/io/urb.h>
#include <cyg/io/mass.h>
#include <cyg/io/hcd.h>



#define OTGClassCode            0x0C032000

#define HOST                    0
#define SLAVE                   1
#define VBUSOFF                 1
#define SRP                     2
#define HNP_A                   3
#define HNP_B                   4

#define OTG_CTRL                0
#define FullSpeedEnable         (1 << 26)
#define HSEBABLEB               (1 << 24)
#define CMPPWDB                 (1 << 23)
#define TEST_SEL                (1 << 22)
#define OSC_SEL                 (1 << 21)
#define PLL_PWD                 (1 << 20)
#define SETRES                  (1 << 17)
#define ENRES                   (1 << 16)
#define SE0                     (1 << 12)
#define DEV_LS                  (1 << 11)
#define DEV_FS                  (1 << 10)
#define HS_DM                   (1 << 9)
#define HS_DP                   (1 << 8)
#define DISCHRGVBUS             (1 << 4)
#define CHRGVBUS                (1 << 3)
#define PWD                     (1 << 2)
#define IDPULLUP                (1 << 1)
#define SEL_HC_DEV              (1 << 0)
//
#define OTG_STATUS              0x04
#define OTG_ENABLE              0x08
#define OTG_CLEAR               0x0C
#define FLDISC                  (1 << 7)
#define FLCONN                  (1 << 6)
#define DISCON                  (1 << 5)
#define SE0OUT                  (1 << 4)
#define VBUSVLD                 (1 << 3)
#define SESSVLD                 (1 << 2)
#define SESSEND                 (1 << 1)
#define ID_DIG                  (1 << 0)
//
#define OTGEN                   (1 << 8)
#define HOSTEN                  (1 << 7)
#define VBUS_CHRG               (1 << 6)
#define VBUS_DISCHRG            (1 << 5)
#define VBUS_DRV                (1 << 4)
#define DM_PULLDOWN             (1 << 3)
#define DP_PULLDOWN             (1 << 2)
#define DM_PULLUP               (1 << 1)
#define DP_PULLUP               (1 << 0)
//#define OTG_STATUS              0x68
#define ID_FLOAT                (1 << 6)
#define ID_GND                  (1 << 5)
#define DP_HI                   (1 << 4)
#define DM_HI                   (1 << 3)
#define VBUS_VLD                (1 << 2)
#define SESS_VLD                (1 << 1)
#define SESS_END                (1 << 0)
// 1 = VBUS voltage is below Session Vaild on device
// 0 = VBUS voltage is above Session Valid on device
//
#define OTG_INTERRUPT_IE        0x6C
#define DP_HI_IE                (1 << 4)
#define DM_HI_IE                (1 << 3)
#define VBUS_VLD_IE             (1 << 2)
#define VSESS_VLD_IE            (1 << 1)
#define VSESS_END_IE            (1 << 0)
//
#define OTG_INTERRUPT_IF        0x70
#define DP_HI_IF                (1 << 4)
#define DM_HI_IF                (1 << 3)
#define VBUS_VLD_IF             (1 << 2)
#define VSESS_VLD_IF            (1 << 1)
#define VSESS_END_IF            (1 << 0)
//
void otg_start(void);
void otg_stop(void);
void check_otg_ID(void);
bool otg_start_srp(void);
bool otg_start_hnp(void);
cyg_uint8 otg_interrupt(void);
cyg_uint32 otg_read(cyg_uint32 m_portBase, cyg_uint32 index);
void otg_write(cyg_uint32 m_portBase, cyg_uint32 index, cyg_uint32 data);
void otg_set(cyg_uint32 m_portBase, cyg_uint32 index, cyg_uint8 data_and, cyg_uint32 data_or);
cyg_uint8 set_hnp_device(struct USB_DEVICE udev, struct hc_driver hcd, int port);
bool otg_hnp(void);
void otg_init(void);
void slave_start_srp(void);
bool host_start_hnp(struct USB_DEVICE udev, struct hc_driver hcd, int port);
void slave_end_srp(void);
cyg_uint8 otg_getStatus(void);
cyg_uint8 otg_getMode(void);

#endif
