//******************************************
// otg.c  
// Functions for otg
//******************************************
//#include <cyg/hal/hal_if.h>			// delays
#include <cyg/io/otg.h>
#include <cyg/io/usberr.h>				//usb err / debug
//=========================================
//Locals
//=========================================
//extern struct HCD_EHCI ehci;
cyg_uint32 dOTGBase;
cyg_uint8 bOTG, bOTGMode;

//=========================================
//Function Prototype
//=========================================
cyg_uint8 otg_getStatus(void){	return bOTG; }
cyg_uint8 otg_getMode(void){	return bOTGMode; }

//=========================================
//Main Functions
//=========================================
cyg_uint32 otg_read(cyg_uint32 m_portBase, cyg_uint32 index)
{
	int value = 0;
				
	HAL_READ_UINT32( (m_portBase+index), value);
	return value;	//return ((cyg_uint32) *(m_portBase+index));
}



void otg_write(cyg_uint32 m_portBase, cyg_uint32 index, cyg_uint32 data)
{
	HAL_WRITE_UINT32((m_portBase+index), data); //TT (cyg_uint32) *(m_portbase+index) = data;
}



void otg_set(cyg_uint32 m_portBase, cyg_uint32 index, cyg_uint8 data_and, cyg_uint32 data_or)
{
        cyg_uint32 data = 0;

				HAL_READ_UINT32( (m_portBase+index), data);
        //data = (m_portBase+index);//data = *(m_portBase+index);
        data &= data_and;
        data |= data_or;
        HAL_WRITE_UINT32((m_portBase+index), data); //TT  *(m_portBase+index) = data;
}

void check_otg_ID(void)
{
        cyg_uint32 data=0;

        data = otg_read(dOTGBase, OTG_STATUS);
        if(data & ID_DIG)
        {
                bOTGMode = SLAVE;
                otg_set(dOTGBase, OTG_CTRL, ~SEL_HC_DEV, SEL_HC_DEV);
        }
        else
        {
                bOTGMode = HOST;
                otg_set(dOTGBase, OTG_CTRL, ~SEL_HC_DEV, 0);
        }

}


void otg_start(void)
{
        otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~(CMPPWDB | SETRES)), CMPPWDB | SETRES);
        otg_write(dOTGBase, OTG_ENABLE, ID_DIG | SESSEND | SESSVLD |
                  VBUSVLD | SE0OUT | DISCON | FLCONN | FLDISC);
}



void otg_stop(void)
{
        otg_write(dOTGBase, OTG_ENABLE, 0);
}


void host_start_srp(void)
{
        otg_set(dOTGBase, OTG_CTRL, ~(CHRGVBUS | DISCHRGVBUS), CHRGVBUS);
        //TTdelay_ms(5);
        otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~SE0), SE0);
        otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~SE0), 0);
}



void slave_start_srp(void)
{
        if(bOTG == VBUSOFF)
        {
                //TTdelay_ms(2);
                otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~(DEV_FS | HS_DP)), DEV_FS);
                //TTdelay_ms(5);
                otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~(DEV_FS | HS_DP)), 0);

                otg_set(dOTGBase, OTG_CTRL, ~(CHRGVBUS | DISCHRGVBUS), CHRGVBUS);
                //TTdelay_ms(2);
                otg_set(dOTGBase, OTG_CTRL, ~(CHRGVBUS | DISCHRGVBUS), 0);
        }
}



void slave_end_srp(void)
{
        otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~(DEV_FS | HS_DP)), DEV_FS);
        //TTdelay_ms(2);
        otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~(DEV_FS | HS_DP)), 0);
}


cyg_uint8 set_hnp_device(struct USB_DEVICE udev, struct hc_driver hcd, int port)
{
        cyg_uint8 data[10] = {0};
        struct usb_otg_descriptor *otgptr = NULL;
        cyg_uint8 result = 0, bOTGFlag=0;
				
        result = usb_control_msg(	udev,
        							hcd, 
									IN,
                                 	USB_RQ_GET_DESCRIPTOR,
                                 	USB_DT_OTG,
                                 	0,
                                 	data,
                                 	3);
        if(!result)
                return false;

        otgptr = (struct usb_otg_descriptor *) data;
        if(otgptr->bDescriptorType == USB_DT_OTG)
                bOTGFlag = otgptr->bmAttributes;

        if(!(bOTGFlag & USB_OTG_HNP))
                return false;

        result = usb_control_msg(	udev,
        							hcd,
        							OUT,
                                 	USB_REQ_SET_FEATURE,
                                 	USB_DEVICE_B_HNP_ENABLE,
                                 	0,
                                 	0,
                                 	2);
        return result;
}


bool host_start_hnp(struct USB_DEVICE udev, struct hc_driver hcd, int port)
{
        //cyg_uint32 data=0;
        cyg_uint8 result=0;

        result = set_hnp_device(udev, hcd, port);
        if(!result)
                return false;

        bOTG = HNP_A;
        otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~(HS_DM | HS_DP)), HS_DM | HS_DP);
        //TTdelay_ms(2);
        otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~(HS_DM | HS_DP)), 0);
        
        return true;
}



void host_end_hnp(void)
{
        bOTGMode = SLAVE;
        otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~SEL_HC_DEV), SEL_HC_DEV);

        otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~(DEV_FS | HS_DP)), DEV_FS);
        //TTdelay_ms(2);
        otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~(DEV_FS | HS_DP)), 0);
}



void slave_hnp(void)
{
        bOTG = HNP_B;
        otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~(HS_DM | HS_DP)), HS_DM | HS_DP);
        //TTdelay_ms(2);
        otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~(HS_DM | HS_DP)), 0);

}



void slave_end_hnp(void)
{
        bOTGMode = HOST;
        otg_set(dOTGBase, OTG_CTRL, ~SEL_HC_DEV, 0);

        otg_set(dOTGBase, OTG_CTRL, ~(CHRGVBUS | DISCHRGVBUS), CHRGVBUS);
        otg_set(dOTGBase, OTG_CTRL, ~(CHRGVBUS | DISCHRGVBUS), 0);
        otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~SE0), SE0);
        otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~SE0), 0);
}



void check_HNP(void)
{
        if (bOTG == HNP_A)
                host_end_hnp();

        if (bOTG == HNP_B)
                slave_end_hnp();
}


cyg_uint8 otg_interrupt(void)
{
        cyg_uint32 data=0;

        data = otg_read(dOTGBase, OTG_STATUS);

        if(data & FLDISC)
        {
                otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~(FullSpeedEnable | HSEBABLEB)), FullSpeedEnable);
                check_HNP();
        }

        if(data & DISCON)
        {
                otg_set(dOTGBase, OTG_CTRL, (cyg_uint8)(~(FullSpeedEnable | HSEBABLEB)), HSEBABLEB);
                check_HNP();
        }

        if(data & SE0OUT)
        {
                if(bOTGMode == HOST)
                {
                        bOTG = SRP;
                }
        }

        if(data & VBUSVLD)
        {
                slave_end_srp();
                bOTG = 0;
        }

        if(data & SESSVLD)
        {
                if(bOTG == SRP)
                        host_start_srp();
                bOTG = 0;
        }

        if(data & SESSEND)
        {
                bOTG = VBUSOFF;
        }

        otg_write(dOTGBase, OTG_CLEAR, data);

        if(data)
                return  true;
        else
                return  false;
}


void otg_init(void)
{
        bOTG = 0;
        dOTGBase = 0x90002700;

        check_otg_ID();
        otg_start();
}
