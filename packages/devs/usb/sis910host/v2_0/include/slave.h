#ifndef _SLAVE_H_
#define _SLAVE_H_

#define SLAVE_CLEAR     0x2710
#define SLAVE_CONTROL   0x2714
#define SLAVE_STATUS    0x2718
#define STOP_IRQ        0x02
#define START_IRQ       0x01

#define SLAVE_MODE      0x271C
#define OTGMODE         0x01
#define SLAVE_DATA      0x2720
#define SLAVE_CHIPID    0x2724
#define SLAVE_VENDORL   0x2728
#define SLAVE_PRODUCT   0x2728
#define SLAVE_UNICODE   0x2730
#define SLAVE_LO_SIZE   0x2734
#define SLAVE_HI_SIZE   0x2738

//Function prototype
void slave_init(void);
cyg_uint8 slave_interrupt(void);
cyg_uint32 slave_read(cyg_uint32 m_portBase, cyg_uint32 index);
void slave_write(cyg_uint32 m_portBase, cyg_uint32 index, cyg_uint32 data);
void slave_set(cyg_uint32 m_portBase, cyg_uint32 index, cyg_uint8 data_and, cyg_uint32 data_or);
void slave_start(void);
void slave_stop(void);

#endif
