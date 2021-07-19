#ifdef OUTPUT_WAV_BY_NET
#include "../include/data_transfer.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <pkgconf/net.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <network.h>



#define _string_(s)	#s

#ifdef CYGHWR_NET_DRIVER_ETH0
#define ETH_IP		_string_(CYGHWR_NET_DRIVER_ETH0_ADDRS_IP)
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
#define ETH_IP		_string_(CYGHWR_NET_DRIVER_ETH1_ADDRS_IP)
#endif

int output_sockfd = -1;
int data_sockfd = -1;

int NET_Data_init(void)
{		
	struct sockaddr_in server;
	int ret;	
	
	init_all_network_interfaces();		
	
	memset((char *)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(_SERVER_PORT);	 
    server.sin_addr.s_addr = inet_addr("172.16.36.65");

	output_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == output_sockfd){
		perror("Create Socket: ");
		return -1;
	}	

	ret = bind(output_sockfd, (struct sockaddr *)&server, sizeof(server));
	if(-1 == ret){
		perror("Bind Failed: "); 
		return -1;
	}
	printf("bind OK! ");
	ret = listen(output_sockfd, 128);
	if(-1 == ret){
		perror("Listen Failed: ");
		return -1;
	}
	printf("Listen OK!\n");
	
	return 0;
}


int NET_Data_transfer(void *buf, int buf_len)
{
	int ret = 0;
	int sndbuf;
	socklen_t optlen;

	if(data_sockfd == -1){
		printf("Create Socket before Transfer\n");
		data_sockfd = accept(output_sockfd, NULL, 0);
		if(-1 == data_sockfd){
			printf("Failed to accept socket!\n");
			return -1;
		}
	}

	sndbuf = 6400;
    optlen = sizeof(sndbuf);
    setsockopt(data_sockfd, SOL_SOCKET, SO_SNDBUF, &sndbuf, optlen);
    sndbuf = 0;
    getsockopt(data_sockfd, SOL_SOCKET, SO_SNDBUF, &sndbuf, &optlen);
    if(6400 != sndbuf){
		printf("set receive buffer failed\n");
		return -1;
    }

	ret = write(data_sockfd, buf, buf_len);
	//printf("write %d bytes data\n", ret);
	
	return ret;
}
#endif

