#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>		// For diagnostic printing.

#include <cyg/io/gateway/io_gateway.h>
#include <cyg/devs/gateway/src/Gateway.h>

inline int calresetsize(int size)
{
    cyg_uint32 send_data = 0;
    if(size & 0xFFFFF000)
    {
    	//more than 4k
    	send_data = size & 0xFFFFF000;
    	diag_printf("GateWay send data %d\n", send_data);
    }
    else
    {
    	send_data = size;
    	diag_printf("GateWay send data %d\n", send_data);
    }
    return send_data;
    
}
static int getchannel(int devid,int RW)
{
    unsigned int chlookup[2][5] = {{CH_READ_VDEC,CH_READ_I2S,CH_READ_USB,CH_READ_SD,CH_READ_MS},
                                    {-1,-1,CH_WRITE_USB,CH_WRITE_SD,CH_WRITE_MS}};
    if(chlookup[RW][devid]!=-1)
    {
        return chlookup[RW][devid];
    }
    else
    {
        diag_printf("GateWay find channel error!!!\n");
    }
}
static bool sis_gateway_read(void* addr,int size,int devid)
{
	int ch = 0;
	Cyg_ErrNo err;
	cyg_gw_info gwinfo = {0};
    cyg_uint32         len;
    cyg_io_handle_t  handle_gateway = NULL;
	cyg_uint32 restsize = 0, offset = 0;
	
	if((addr == NULL) || (size <= 0) || (devid > 4)||(devid < 0))
	{
	    return false;
	}
	ch = getchannel(devid,READ);
	err = cyg_io_lookup("/dev/GATEWAY",&handle_gateway);
	if(ENOERR != err || NULL == handle_gateway) 
    {
        diag_printf("gateway IO cyg_io_lookup error!!! \n");
        return false;
    }
	if(!gateway_channel_query(ch,READ))
	{
	    gateway_reset_channel(ch, READ);
	}
	cyg_mutex_lock(&ch_mutex_lock[ch] );
    while(restsize > 0)
    {
        gwinfo.base = addr + offset;
        offset = calresetsize(restsize);//get the next size
        restsize -= offset;
	    gwinfo.size = offset;
	    gwinfo.devid = devid;
	    len = sizeof(gwinfo);
	    err = cyg_io_read(handle_gateway,&gwinfo,&len);     
        if(ENOERR != err) 
        {
            diag_printf("gateway IO cyg_io_read error!!! \n");
            return false;
        }   
    }
    
    return true;
        	
}

static bool sis_gateway_write(void* addr,int size,int devid)
{
    int ch = 0;
	cyg_io_handle_t  handle_gateway = NULL;
	Cyg_ErrNo err;
	cyg_gw_info gwinfo = {0};
    cyg_uint32         len;
	cyg_uint32 restsize = 0, offset = 0;
	if((addr ==NULL) || (size<=0) || (devid > 3)||(devid < 1))
	{
	    return false;
	}
	ch = getchannel(devid,WRITE);
	restsize = size;
	    
    cyg_mutex_lock(&ch_mutex_lock[ch + MUTEX_CH_SHT] );
			
	err = cyg_io_lookup("/dev/GATEWAY",&handle_gateway);
	
	if(ENOERR != err || NULL == handle_gateway) 
    {
        diag_printf("gateway IO cyg_io_lookup error!!! \n");
        return false;
    }
	if(!gateway_channel_query(ch,WRITE))
	{
	    gateway_reset_channel(ch, WRITE);
	}
    while(restsize > 0)
    {
        gwinfo.base = addr + offset;
        offset = calresetsize(restsize);//get the next size
        restsize -= offset;
	    gwinfo.size = offset;
	    gwinfo.devid = devid;
	    len = sizeof(gwinfo);
	    err = cyg_io_write(handle_gateway,&gwinfo,&len);     
        if(ENOERR != err) 
        {
           diag_printf("gateway IO cyg_io_write error!!! \n");
           return false;
        }
    }
    return true;
}

static bool gateway_channel_query(int devid,int RW)                  
{  
    cyg_io_handle_t  handle_gateway = NULL;
    Cyg_ErrNo err;
    char* reg_status = NULL;
    cyg_gw_info gwinfo = {0};
    cyg_uint32 bread, len;
    
    
    err = cyg_io_lookup("/dev/GATEWAY",&handle_gateway);
	if(ENOERR != err || NULL == handle_gateway) 
    {
        diag_printf("gateway IO cyg_io_lookup error!!! \n");
        return false;
    }
    bread = RW?QUERY_CHANNEL_STATUS_READ:QUERY_CHANNEL_STATUS_WRITE;
    gwinfo.base = reg_status;
	gwinfo.size = 1;
	gwinfo.devid = devid;
    len = sizeof(gwinfo);
   
    err = cyg_io_get_config(handle_gateway ,bread , &gwinfo ,&len );     
    if(ENOERR != err) 
    {
           diag_printf("gateway IO cyg_io_get_config error!!! \n");
           return false;
    }
    return reg_status;
} 

static Cyg_ErrNo gateway_stop(int devid,int RW)                  
{
    cyg_io_handle_t  handle_gateway = NULL;
    Cyg_ErrNo err;
    cyg_gw_info gwinfo = {0};
    cyg_uint32 bread, len;
    int ch = getchannel(devid,RW);
    bread = RW?GATEWAY_STOP_CHANNEL_READ:GATEWAY_STOP_CHANNEL_WRITE;
    
    err = cyg_io_lookup("/dev/GATEWAY",&handle_gateway);
	if(ENOERR != err || NULL == handle_gateway) 
    {
        diag_printf("gateway IO cyg_io_lookup error!!! \n");
        return false;
    }
    gwinfo.base = NULL;
	gwinfo.size = 1;
	gwinfo.devid = devid;
    len = sizeof(gwinfo);
    err = cyg_io_set_config(handle_gateway ,bread ,&gwinfo ,&len );
    if(ENOERR != err) 
    {
           diag_printf("gateway IO cyg_io_set_config error!!! \n");
           return false;
    }
    return true;  
} 

static Cyg_ErrNo gateway_reset_channel(int devid,int RW)                  
{
    cyg_io_handle_t  handle_gateway = NULL;
    Cyg_ErrNo err;
    cyg_gw_info gwinfo = {0};
    cyg_uint32 bread, len;
    
    bread = RW?GATEWAY_RESET_CHANNEL_READ:GATEWAY_RESET_CHANNEL_WRITE;
    
    err = cyg_io_lookup("/dev/GATEWAY",&handle_gateway);
	if(ENOERR != err || NULL == handle_gateway) 
    {
        diag_printf("gateway IO cyg_io_lookup error!!! \n");
        return false;
    }
    gwinfo.base = NULL;
	gwinfo.size = 1;
	gwinfo.devid = devid;
    len = sizeof(gwinfo);
    err = cyg_io_set_config(handle_gateway ,bread ,&gwinfo ,&len );
    if(ENOERR != err) 
    {
           diag_printf("gateway IO cyg_io_set_config error!!! \n");
           return false;
    }
    return true;
} 
