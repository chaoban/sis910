//-------------------------------------------------------------------------
//=========================================================================
//####### DESCRIPTIONBEGIN #######
//
// sis910_kbd.c
//
// Author:
// Date:
// Purpose:      SiS910 HAL board support.
// Description:  Implementations of Keyboard driver.
//
//=========================================================================
// SIS910 KEYBOARD DRIVER

#include <cyg/infra/diag.h>		// For diagnostic printing.
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_intr.h>
#include <pkgconf/devs_kbd_sis910.h>
#include <stdio.h>
#include <string.h>
#include <cyg/fileio/fileio.h>  // For select() functionality
#include <cyg/io/devtab.h>

static cyg_selinfo      kbd_select_info;
static cyg_bool         kbd_select_active;

static Cyg_ErrNo kbd_read(cyg_io_handle_t handle,
                          void *buffer,
                          cyg_uint32 *len);
static cyg_bool  kbd_select(cyg_io_handle_t handle,
                            cyg_uint32 which,
                            cyg_addrword_t info);
static Cyg_ErrNo kbd_set_config(cyg_io_handle_t handle,
                                cyg_uint32 key,
                                const void *buffer,
                                cyg_uint32 *len);
static Cyg_ErrNo kbd_get_config(cyg_io_handle_t handle,
                                cyg_uint32 key,
                                void *buffer,
                                cyg_uint32 *len);
static bool      kbd_init(struct cyg_devtab_entry *tab);
static Cyg_ErrNo kbd_lookup(struct cyg_devtab_entry **tab,
                            struct cyg_devtab_entry *st,
                            const char *name);

CHAR_DEVIO_TABLE(sis910_kbd_handlers,
                 NULL,                                   // Unsupported write() function
                 kbd_read,
                 kbd_select,
                 kbd_get_config,
                 kbd_set_config);

CHAR_DEVTAB_ENTRY(sis910_kbd_device,
                  CYGDAT_DEVS_KBD_SIS910_NAME,
                  NULL,                                   // Base device name
                  &sis910_kbd_handlers,
                  kbd_init,
                  kbd_lookup,
                  NULL);

//#define MAX_EVENTS CYGNUM_DEVS_KBD_SIS910_EVENT_BUFFER_SIZE
#define MAX_EVENTS 1
static int   num_events = 0;
static int   _event_put = 0, _event_get = 0;
static unsigned char _events[MAX_EVENTS];
static unsigned char _event_test;
static bool _is_input = 0;

static bool _is_open = false;

#define STACK_SIZE CYGNUM_HAL_STACK_SIZE_TYPICAL
static char kbd_scan_stack[STACK_SIZE];
static cyg_thread kbd_scan_thread_data;
static cyg_handle_t kbd_scan_thread_handle;

static cyg_handle_t intr_gpio_handle;
static cyg_interrupt intr_gpio_object;
cyg_priority_t GPIO_PRIOR = 18;

#define TRIG_MODE 	TRIG_FAIL	//0x10
#define GPIO_INTR	CYGNUM_HAL_INTERRUPT_GPIO

#ifdef CYG_HAL_SIS910B
static cyg_uint32 enable = (RGP0|RGP1|RGP2); 							// 6 buttons in 3 GPIO.
static cyg_uint32 trig =  ((TRIG_MODE<<SGP0)|(TRIG_MODE<<SGP1)|(TRIG_MODE<<SGP2));
#else
static cyg_uint32 enable = (RGP0|RGP1|RGP2|RGP3|RGP4|RGP5);             // 6 buttons in 6 GPIO.
static cyg_uint32 trig =  ((TRIG_MODE<<SGP0)|(TRIG_MODE<<SGP1)|(TRIG_MODE<<SGP2)|
                		  (TRIG_MODE<<SGP3)|(TRIG_MODE<<SGP4)|(TRIG_MODE<<SGP5));
#endif

//chaoban test
int KeyBoardGlobal = 0;


void
enable_irq(cyg_uint32 i)
{
	cyg_uint32 _VAL, mask;

	HAL_READ_UINT32(IO_IRQ_EN, _VAL);
    mask = (1 << (i + 16));
    _VAL |= mask;
    HAL_WRITE_UINT32(IO_IRQ_EN, _VAL);
}

void
disable_irq(cyg_uint32 i)
{
	cyg_uint32 _VAL, mask;

	HAL_READ_UINT32(IO_IRQ_EN, _VAL);
    mask = (1 << (i + 16));
    _VAL &= ~mask;
    HAL_WRITE_UINT32(IO_IRQ_EN, _VAL);
}

void
gpio_initial(cyg_uint32 enable, cyg_uint32 trig)
{
	cyg_uint32 _VAL;

	HAL_READ_UINT32(REG_GPIO_Enable, 	_VAL);
	_VAL |= enable;
	HAL_WRITE_UINT32(REG_GPIO_Enable, 	_VAL);				// GPIO Enable, 	 3740

	HAL_READ_UINT32(REG_GPIO_INTR, 		_VAL);
	_VAL |= enable;
	HAL_WRITE_UINT32(REG_GPIO_INTR, 	_VAL);				// Interrupt enable, 3754

	HAL_READ_UINT32(REG_GPIO_TRIGGER, 	_VAL);
	_VAL |= trig;
	HAL_WRITE_UINT32(REG_GPIO_TRIGGER, 	_VAL);				// Rising edge mode, 3758

	HAL_WRITE_UINT32(REG_GPIO_EnOut, 	0x00);				// = INPUT enable,  374c
	HAL_WRITE_UINT32(REG_GPIO_CLRINTR, 	0x00);				// Disable clear iqr
}

void
gpio_clear_irq(cyg_uint32 enable)
{
	cyg_uint32 _VAL;

	HAL_READ_UINT32(REG_GPIO_CLRINTR, 	_VAL);
	_VAL |= enable;
	HAL_WRITE_UINT32(REG_GPIO_CLRINTR, 	_VAL);

	HAL_READ_UINT32(REG_GPIO_CLRINTR, 	_VAL);
	_VAL &= ~enable;
	HAL_WRITE_UINT32(REG_GPIO_CLRINTR, 	_VAL);
}

static void
kbd_scan(cyg_addrword_t param)
{
    unsigned char *ev;
	cyg_uint32 _VAL = 0;

    enable_irq(GPIO_INTR);
    gpio_initial(enable, trig);

    while (true)
    {
        if (num_events < MAX_EVENTS)
        {
            num_events++;
            ev = &_events[_event_put++];
            if (_event_put == MAX_EVENTS)
            {
                _event_put = 0;
            }

    		do
    		{
    			HAL_READ_UINT32(REG_GPIO_INTStat, _VAL);
    		}
    		while(!(_VAL & enable)) ;			//INT state will switch to 0 after some cycles. 90003760

    		gpio_clear_irq(enable);

        	switch(_VAL)
        	{
        		case 1:
        			*ev = 0x1;
        			break;
        		case 2:
        			*ev = 0x2;
        			break;
        		case 4:
        			*ev = 0x4;
        			break;
        		case 8:
        			*ev = 0x8;
        			break;
        		case 16:
        			*ev = 0x10;
        			break;
        		case 32:
        			*ev = 0x20;
        			break;
        		default:
        			diag_printf("sis910_kbd : I don't know what you pressed.\n");
        			*ev = 0x0;
        			break;
        	}

            if (kbd_select_active)
            {
                kbd_select_active = false;
                cyg_selwakeup(&kbd_select_info);
            }
        }
    }
}

static Cyg_ErrNo
kbd_read(cyg_io_handle_t handle, void *buffer, cyg_uint32 *len)
{
    unsigned char *ev;
    int tot = *len;
    unsigned char *bp = (unsigned char *)buffer;

    cyg_scheduler_lock();  // Prevent interaction with DSR code
    *len = 0;
    if(_is_input)
    {
        ev = &_event_test;
        _is_input = 0;
        *len = 1;
        memcpy(bp, ev, sizeof(*ev));


    }
    cyg_scheduler_unlock(); // Allow DSRs again
    //while (tot >= sizeof(*ev))
    //{
    //    if (num_events > 0)
    //    {
    //        ev = &_events[_event_get++];
    //
    //        if (_event_get == MAX_EVENTS)
    //        {
    //            _event_get = 0;
    //        }
    //        memcpy(bp, ev, sizeof(*ev));
    //        bp += sizeof(*ev);
    //        tot -= sizeof(*ev);
    //        num_events--;
    //    }
    //    else
    //    {
    //        break;  // No more events
    //    }
    //}
    //cyg_scheduler_unlock(); // Allow DSRs again
    //*len -= tot;
    return ENOERR;
}

static cyg_bool
kbd_select(cyg_io_handle_t handle, cyg_uint32 which, cyg_addrword_t info)
{
    if (which == CYG_FREAD)
    {
        cyg_scheduler_lock();  // Prevent interaction with DSR code
        if (num_events > 0)
        {
            cyg_scheduler_unlock();  // Reallow interaction with DSR code
            return true;
        }
        if (!kbd_select_active)
        {
            kbd_select_active = true;
            cyg_selrecord(info, &kbd_select_info);
        }
        cyg_scheduler_unlock();  // Reallow interaction with DSR code
    }
    return false;
}

static Cyg_ErrNo
kbd_set_config(cyg_io_handle_t handle, cyg_uint32 key, const void *buffer, cyg_uint32 *len)
{
    return EINVAL;
}

static Cyg_ErrNo
kbd_get_config(cyg_io_handle_t handle, cyg_uint32 key, void *buffer, cyg_uint32 *len)
{
    return EINVAL;
}

static void
kbd_decode(void)
{
    unsigned char *ev;
	cyg_uint32 _VAL = 0;
	//printf("------------------kbd_decode----------------\n");

    ev = &_event_test;

    HAL_READ_UINT32(REG_GPIO_INTStat, _VAL);	//INT state will switch to 0 after some cycles. 90003760

#ifdef CYG_HAL_SIS910B
    /* 910A: 6 buttons in 6 GPIO Pins;
       910B: 6 buttons in 3 GPIO Pins.

                    G_0    G_1    G_2
       Button 1:     1      0      0
       Button 2:     0      1      0
       Button 3:     1      1      0
       Button 4:     0      0      1
       Button 5:     1      0      1
       Button 6:     0      1      1
    */
    _VAL &= 0x7;
    _VAL = 1 << (_VAL - 1);
#else
    _VAL &= 0x3f;
#endif

    gpio_clear_irq(enable);

    _is_input = 1;

    switch(_VAL)
    {
      	case 1:
       		*ev = 0x1;
       		diag_printf("sis910_kbd : Button 1.\n");
       		KeyBoardGlobal = 1; //chaoban test
       		break;
       	case 2:
       		*ev = 0x2;
       		diag_printf("sis910_kbd : Button 2.\n");
       		KeyBoardGlobal = 2; //chaoban test
       		break;
       	case 4:
       		*ev = 0x4;
       		diag_printf("sis910_kbd : Button 3.\n");
       		KeyBoardGlobal = 3; //chaoban test
       		break;
       	case 8:
       		*ev = 0x8;
       		diag_printf("sis910_kbd : Button 4.\n");
       		KeyBoardGlobal = 4; //chaoban test
       		break;
       	case 16:
       		*ev = 0x10;
       		diag_printf("sis910_kbd : Button 5.\n");
       		KeyBoardGlobal = 5; //chaoban test
       		break;
       	case 32:
       		*ev = 0x20;
       		diag_printf("sis910_kbd : Button 6.\n");
       		KeyBoardGlobal = 6; //chaoban test
       		break;
       	default:
        	_is_input = 0;
        	*ev = 0x0;
        	KeyBoardGlobal = 0; //chaoban test
        	break;
    }

    if (kbd_select_active)
    {
        kbd_select_active = false;
        cyg_selwakeup(&kbd_select_info);
    }

//    if (num_events < MAX_EVENTS)
//    {
//    	num_events++;
//        ev = &_events[_event_put++];
//        if (_event_put == MAX_EVENTS)
//        {
//        	_event_put = 0;
//        }
//
//    	do
//    	{
//    		HAL_READ_UINT32(REG_GPIO_INTStat, _VAL);
//    	}
//    	while(!(_VAL & enable)) ;			//INT state will switch to 0 after some cycles. 90003760
//
//    	gpio_clear_irq(enable);
//
//        switch(_VAL)
//        {
//        	case 1:
//        		*ev = 0x1;
//        		diag_printf("sis910_kbd : Button 1.\n");
//        		break;
//        	case 2:
//        		*ev = 0x2;
//        		diag_printf("sis910_kbd : Button 2.\n");
//        		break;
//        	case 4:
//        		*ev = 0x4;
//        		diag_printf("sis910_kbd : Button 3.\n");
//        		break;
//        	case 8:
//        		*ev = 0x8;
//        		diag_printf("sis910_kbd : Button 4.\n");
//        		break;
//        	case 16:
//        		*ev = 0x10;
//        		diag_printf("sis910_kbd : Button 5.\n");
//        		break;
//        	case 32:
//        		*ev = 0x20;
//        		diag_printf("sis910_kbd : Button 6.\n");
//        		break;
//        	default:
//        		diag_printf("sis910_kbd : I don't know what you pressed.\n");
//        		*ev = 0x0;
//        		break;
//        }
//
//        if (kbd_select_active)
//        {
//            kbd_select_active = false;
//            cyg_selwakeup(&kbd_select_info);
//        }
//    }
//    else {
//    	//TODO: What I should do ?
//    	diag_printf("sis910_kbd : Oh! num_events >= MAX_EVENTS !!\n");
//    	diag_printf("sis910_kbd : Don't press again, or I will kill you !!\n");
//    	num_events--;
//    }
}

static cyg_uint32
sis910_kdb_isr( cyg_vector_t vector, cyg_addrword_t data)
{
//	diag_printf("Buttons ISR by GPIO\n");

	// Block this interrupt from occurring until
	// the DSR completes.
	cyg_drv_interrupt_mask_intunsafe(vector);

	// Tell the processor that we have received
	// the interrupt.
	cyg_drv_interrupt_acknowledge(vector);

	kbd_decode();				// Detect which button be pressed.
								// then, TODO something.

	// Tell the kernel that chained interrupt processing
	// is done and the DSR needs to be executed next.
	//return(CYG_ISR_HANDLED|CYG_ISR_CALL_DSR);
	return CYG_ISR_HANDLED;
}

static void
sis910_kdb_dsr( cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
	diag_printf("Buttons DSR by GPIO\n");
//	diag_printf("\n");

	// Signal the thread to run for further processing.
//	cyg_semaphore_post( &data_ready );

	cyg_drv_interrupt_acknowledge(vector);

	// Allow this interrupt to occur again.
	cyg_drv_interrupt_unmask(vector);
}

static bool
kbd_init(struct cyg_devtab_entry *tab)
{
    cyg_selinit(&kbd_select_info);

    enable_irq(GPIO_INTR);
    gpio_initial(enable, trig);

    // Create GPIO interrupt.
	cyg_drv_interrupt_create
	(
		GPIO_INTR,
		GPIO_PRIOR,
		0,	//data
		sis910_kdb_isr,
		sis910_kdb_dsr,
		&intr_gpio_handle,
		&intr_gpio_object
	);

// Attach the interrupt created to the vector.
	cyg_drv_interrupt_attach(intr_gpio_handle);

// Unmask the interrupt we just configured.
	cyg_drv_interrupt_mask(GPIO_INTR);	//chaoban test added
	cyg_drv_interrupt_unmask(GPIO_INTR);

    return true;
}

static Cyg_ErrNo
kbd_lookup(struct cyg_devtab_entry **tab, struct cyg_devtab_entry *st, const char *name)
{
    if (!_is_open) {
        _is_open = true;
//        cyg_thread_create(18,                      // Priority
//                          kbd_scan,                // entry
//                          0,                       // entry parameter
//                          "Keyboard scan",         // Name
//                          &kbd_scan_stack[0],      // Stack
//                          STACK_SIZE,              // Size
//                          &kbd_scan_thread_handle, // Handle
//                          &kbd_scan_thread_data    // Thread data structure
//        );
// chaoban test
//        cyg_thread_resume(kbd_scan_thread_handle);    // Start it
}
    return ENOERR;
}
