//#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h> 	// base types
#include <cyg/io/slave.h>
#include <cyg/io/otg.h>
#include <cyg/io/usberr.h>			//usb err / debug

//extern cyg_uint8 bOTG;
cyg_uint32 dSlaveBase;

cyg_uint32 slave_read(cyg_uint32 m_portBase, cyg_uint32 index)
{
				int value = 0;
				
				HAL_READ_UINT32( (m_portBase+index), value);
        return value;	
        //return ((cyg_uint32) *(m_portBase+index));
}



void slave_write(cyg_uint32 m_portBase, cyg_uint32 index, cyg_uint32 data)
{
        HAL_WRITE_UINT32((m_portBase+index), data);//(cyg_uint32) *(m_portbase+index) = data;
}



void slave_set(cyg_uint32 m_portBase, cyg_uint32 index, cyg_uint8 data_and, cyg_uint32 data_or)
{
        cyg_uint32 data=0;

        HAL_READ_UINT32( (m_portBase+index), data);//data = *(m_portBase+index);
        data &= data_and;
        data |= data_or;
        HAL_WRITE_UINT32((m_portBase+index), data);//*(m_portBase+index) = data;
}



void slave_start(void)
{
        slave_set(dSlaveBase, SLAVE_CONTROL, ~(START_IRQ | STOP_IRQ), START_IRQ | STOP_IRQ);
}



void slave_stop(void)
{
        slave_set(dSlaveBase, SLAVE_CONTROL, ~(START_IRQ | STOP_IRQ), 0);
}



cyg_uint8 slave_interrupt(void)
{
        cyg_uint32 data;

        data = slave_read(dSlaveBase, SLAVE_STATUS);

        if(data & START_IRQ)
        {
					//Data in
        }

        if(data & STOP_IRQ)
        {
					//Data out
        }

        slave_set(dSlaveBase, SLAVE_CLEAR, ~(START_IRQ | STOP_IRQ), data);

        if(data)
                return  true;
        else
                return  false;
}



void slave_init(void)
{
        dSlaveBase = 0x90000000;

        if(otg_getStatus() == HOST)
                slave_set(dSlaveBase, SLAVE_MODE, ~OTGMODE, OTGMODE);
        else
                slave_set(dSlaveBase, SLAVE_MODE, ~OTGMODE, 0);

        slave_start();
}
