#include "phy_eth_server.h"

void *server_entry()
{
	int connfd_new;
	socklen_t client_len;
	pthread_t recv_tid;
	struct timeval timeout;
	struct sockaddr_in serv_addr , client_addr;
	
	printf("server_entry...\n");

	if((socket_dev.listen_fd = socket(AF_INET , SOCK_STREAM , 0)) == -1) {
		perror("socket error.\n");
		exit(1);
	}

	bzero(&serv_addr , sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(CONNECT_PORT);

	if(bind(socket_dev.listen_fd , (struct sockaddr *)&serv_addr , sizeof(serv_addr)) < 0) {
		perror("bind error.\n");
		exit(1);
	}

	if(listen(socket_dev.listen_fd , SERVER_LISTEN_MAX) < 0) {
		perror("listen error.\n");
		exit(1);
	}

	while (1) {
		client_len = sizeof(client_addr);
		if((connfd_new = accept(socket_dev.listen_fd , (struct sockaddr *)&client_addr , &client_len)) < 0) {
			perror("accept error.\n");
			exit(1);
		}
		if (socket_dev.conn_fd != -1) {
			close(socket_dev.conn_fd);
		}
		socket_dev.conn_fd = connfd_new;
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		if (setsockopt(socket_dev.conn_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == -1) {
			printf("setsockopt:%08x  SO_RCVTIMEO error\n", socket_dev.conn_fd);						
		}
		if (setsockopt(socket_dev.conn_fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) == -1) {
			printf("setsockopt:%08x  SO_SNDTIMEO error\n", socket_dev.conn_fd);						
		}
		socket_dev.link_status = SYNC_LINK_CONNECTED;
		printf("server: got connection from %s\n", inet_ntoa(client_addr.sin_addr));
	}
}