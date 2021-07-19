#include <stdio.h>
#include <string.h>
#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>		// For diagnostic printing.
#include <cyg/io/iis/io_iis.h>

int iis_set_value(cyg_uint32 bitrate, cyg_uint32 samplerate)
{
	Cyg_ErrNo err;
	cyg_io_handle_t iis_handle = NULL;
	cyg_uint32 buffer[ 1 ] = {-1};
	cyg_uint32 len = 4;//4 byte

	err = cyg_io_lookup("/dev/SISIIS", &iis_handle);
	if(ENOERR != err || NULL == iis_handle) 
    {
        diag_printf("IIS IO cyg_io_lookup error!!! \n");
        return -1;
    }
    
    buffer[0] = bitrate;
    err = cyg_io_set_config(iis_handle, FLAG_BITRATE, buffer, &len);
    if(ENOERR != err) 
    {
    	diag_printf("IIS IO cyg_io_set_config(FLAG_BITRATE) error!! \n");
    }
    buffer[0] = samplerate;
    err = cyg_io_set_config(iis_handle, FLAG_SAMPLERATE, buffer, &len);
    if(ENOERR != err) 
    {
    	diag_printf("IIS IO cyg_io_set_config(FLAG_SAMPLERATE) error!! \n");
    }
    return err;
}
int iis_send_data(cyg_uint32 addr, cyg_uint32 size)
{
	Cyg_ErrNo err;
	cyg_io_handle_t iis_handle = NULL;
	cyg_uint32 buffer[ 1 ] = {-1};
	cyg_uint32 len = 4;//4 byte

	err = cyg_io_lookup("/dev/SISIIS", &iis_handle);
	if(ENOERR != err || NULL == iis_handle) 
    {
        diag_printf("IIS IO cyg_io_lookup error!!! \n");
        return -1;
    }
    buffer[0] = addr;
    err = cyg_io_set_config(iis_handle, FLAG_BUF_ADDR, buffer, &len);
    if(ENOERR != err) 
    {
    	diag_printf("IIS IO cyg_io_set_config(FLAG_BUF_ADDR) error!! \n");
    }
    buffer[0] = size;
    err = cyg_io_set_config(iis_handle, FLAG_BUF_SIZE, buffer, &len);
    if(ENOERR != err) 
    {
    	diag_printf("IIS IO cyg_io_set_config(FLAG_BUF_SIZE) error!! \n");
    }    
    err = cyg_io_bwrite(iis_handle, buffer, &len, 0); //set gw
    if(ENOERR != err) 
    {
    	diag_printf("IIS IO cyg_io_write error!! \n");
    }
    return 0;
}

cyg_uint32 iis_get_status(cyg_uint32 key)
{
	Cyg_ErrNo err;
	cyg_io_handle_t iis_handle = NULL;
	cyg_uint32 buffer[ 1 ] = {-1};
	cyg_uint32 len = 4;//4byte
	err = cyg_io_lookup("/dev/SISIIS", &iis_handle);
	if(ENOERR != err || NULL == iis_handle) 
    {
        diag_printf("IIS IO cyg_io_lookup error!!! \n");
        return -1;
    }
    
    err = cyg_io_get_config(iis_handle, key, buffer, &len);
    if(ENOERR != err) 
    {
    	diag_printf("IIS IO cyg_io_get_config error!! \n");
    }
    return buffer[0];
}