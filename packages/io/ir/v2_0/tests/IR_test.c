//IR_test
#include <cyg/io/ir/io_ir.h>
#include <cyg/kernel/kapi.h>	// Kernel API.
#include <cyg/infra/diag.h>		// For diagnostic printing.
//
// This is the main starting point for our
// example application.
//
void cyg_user_start( void )
{
	int test_code = -1;
	int i = 0;
	for(i = 0; i < 1000; i++)
	{
		test_code = sisir_get_ircode();
		diag_printf("The %dth test get code is :%d/n", i , test_code);
	}
	
}