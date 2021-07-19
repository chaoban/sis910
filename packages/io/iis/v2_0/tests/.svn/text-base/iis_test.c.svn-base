//IIS_test
#include <cyg/io/iis/io_iis.h>
#include <cyg/kernel/kapi.h>	// Kernel API.
#include <cyg/infra/diag.h>		// For diagnostic printing.
//
// This is the main starting point for our
// example application.
//
void cyg_user_start( void )
{
	int i = 0;

	for(i = 0; i < 10; i++)
	{
		iis_debug(FLAG_IIS_ENABLE);
		iis_debug(FLAG_IIS_BCLK_MODE);
		iis_debug(FLAG_IIS_LR_MODE);
		iis_debug(FLAG_IIS_MCLK_MODE);
		
		
		
		iis_enable();//enable iis
		
		iis_debug(FLAG_IIS_ENABLE);
		
		iis_disable();//iis disable
		iis_debug(FLAG_IIS_ENABLE);
		
		iis_enable();//enable iis
		
		
		iis_setup_parameter(FLAG_BITRATE, 24);
		iis_debug(FLAG_IIS_BCLK_MODE);
		iis_setup_parameter(FLAG_BITRATE, 16);
		iis_debug(FLAG_IIS_BCLK_MODE);
		
		iis_debug(FLAG_BUF_EMPTY);
		iis_debug(FLAG_BUF_FULL);
		iis_debug(FLAG_SEND_DATA_END);
		iis_debug(FLAG_DEBUG_IIS);
		iis_debug(FLAG_IIS_COUNT);
		iis_debug(FLAG_PRESENT_STATE);
	}
	

	
}