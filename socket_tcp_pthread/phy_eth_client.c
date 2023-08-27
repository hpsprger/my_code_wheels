#include "phy_eth_client.h"

extern char server_ip[];

void *client_entry()
{
    struct sockaddr_in serv_addr;
	struct tcp_info info; 
	int len=sizeof(info);
	struct timeval timeout;

	printf("client_entry...\n");

	while (1) {
		socket_dev.link_status = SYNC_LINK_DISCONNECTED;
		if((socket_dev.conn_fd = socket(AF_INET , SOCK_STREAM , 0)) == -1)
		{
			perror("socket error");
			exit(1);
		}

		bzero(&serv_addr , sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(CONNECT_PORT);
		if(inet_pton(AF_INET , server_ip , &serv_addr.sin_addr) < 0)
		{
			printf("inet_pton error\n");
			close(socket_dev.conn_fd);
			continue;
		}

		if(connect(socket_dev.conn_fd , (struct sockaddr *)&serv_addr , sizeof(serv_addr)) < 0)
		{
			perror("connect error");
			close(socket_dev.conn_fd);
			sleep(1);
			continue;
		}

		printf("client connect ok\n");

		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		if (setsockopt(socket_dev.conn_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == -1) 
		{
			printf("setsockopt:%08x  SO_RCVTIMEO error\n", socket_dev.conn_fd);						
		}
		if (setsockopt(socket_dev.conn_fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) == -1) 
		{
			printf("setsockopt:%08x  SO_SNDTIMEO error\n", socket_dev.conn_fd);						
		}
		socket_dev.link_status = SYNC_LINK_CONNECTED;
		while (1) {
			getsockopt(socket_dev.conn_fd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
			if (info.tcpi_state == TCP_ESTABLISHED) {
				sleep(1);
				continue;
			} else {
				socket_dev.link_status = SYNC_LINK_DISCONNECTED;
				close(socket_dev.conn_fd);
				break;
			}
		}
	}
}