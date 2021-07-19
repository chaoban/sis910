#include <stdio.h>
#include <string.h>
#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>		// For diagnostic printing.

int sisir_get_ircode(void)
{
	Cyg_ErrNo err;
	cyg_io_handle_t sisir_handle = NULL;
	int code = -1;
	static char buffer[ 1 ] = {-1};
	cyg_uint32 len = 1;
	
    err = cyg_io_lookup("/dev/SISIR", &sisir_handle);
    if(ENOERR != err || NULL == sisir_handle) 
    {
        diag_printf("IR IO cyg_io_lookup error!!! \n");
        return -1;
    }
    err = cyg_io_bread(sisir_handle, buffer, &len, 0);
    
    if(ENOERR != err) 
    {
    	diag_printf("IR IO cyg_io_bread error!! \n");
    }
    
    code = buffer[0];
    
    return code;
}

//#ifdef IR_FPGA_TEST
int sisir_polling(void)
{
	//at sis910 fpga target
	//there is no interrupt to test
	//add this function to polling 
	//and to call isr to get code
	//need remove if real chip support interrupt
	cyg_uint32 ir_diff = 0, wait_irq = 0;
	int i = 0;
	int result[32] = {0};
	unsigned short  dataIR = 0;
	
	for(i = 0; i< 32; i++)
	{
		result[i] = 0;
	}
	
	i = 0;
	
	HAL_WRITE_UINT32(IR_SYS_STATUS, 0x4);//clear irq

	while( i<32)
	{
		//CHAOBAN TEST
     	//diag_printf("wait_irq is %x!!!\n",wait_irq);
     	do {
			HAL_READ_UINT32(IO_IRQ_WAIT, wait_irq);
		} 
		while (!(wait_irq & 0x2000));

//		if(wait_irq & 0x2000)
		{
			HAL_READ_UINT32(IR_IRDIFF, ir_diff);//read diff
			result[i] = ir_diff;
		
			HAL_WRITE_UINT32(IR_SYS_STATUS, 0x4);//clear irq
		
			wait_irq &= ~0x2000;
			ir_diff = 0;
			i++;
		}
	}
		
	for(i = 0;i <32; i++)
	{
		diag_printf("%d,", result[i]);
	}
	diag_printf("\n");
	
	/*
	//diag_printf("test end!!, the i is %d\n", i);
	diag_printf("(");
	for(i = 3; i<11; i++)
	{
		if(result[i]>150)
		{
			diag_printf("1");
		}
		else
		{
			diag_printf("0");
		}
	}
	diag_printf(",");
	for(i = 11; i<19; i++)
	{
		if(result[i]>150)
		{
			diag_printf("1");
		}
		else
		{
			diag_printf("0");
		}
	}
	diag_printf(",");
	for(i = 19; i<27; i++)
	{
		if(result[i]>150)
		{
			diag_printf("1");
			dataIR |= 1<< (i-19);
		}
		else
		{
			diag_printf("0");
		}
	}
	diag_printf(")\n");
	*/
	return dataIR;
}

//#endif //IR_FPGA