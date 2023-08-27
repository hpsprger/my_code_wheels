#include "phy_eth_common.h"

int push_msg_fifo(link_msg_fifo *pfifo,  link_msg *pmsg)
{
	unsigned int msg_total_len;
	unsigned int cur_len;

	if (pfifo == NULL || pmsg == NULL) {
		printf("push_msg_fifo pointer null\n");
		return -1;
	}

	if (pmsg->payload == NULL) {
		printf("push_msg_fifo payload null\n");
		return -1;
	}

	msg_total_len = sizeof(pmsg->head) + pmsg->head.len;

	if (pfifo->depth < msg_total_len) {
		printf("push_msg_fifo  free space not enough \n");
		return -1; /* free space not enough */
	}
	pthread_mutex_lock(&pfifo->mutex);
	/* aroud or fist time is equal*/
	if (pfifo->wr <= pfifo->rd) {
		memcpy(&(pfifo->buffer[pfifo->wr]), &pmsg->head, sizeof(pmsg->head));
		memcpy(&(pfifo->buffer[pfifo->wr + sizeof(pmsg->head)]), pmsg->payload, pmsg->head.len);
		pfifo->wr += msg_total_len;
		pfifo->depth -= msg_total_len;
	} else {
		/* step1: copy head */
		cur_len = pfifo->depth_max - pfifo->wr; /* cur_len: wr to depth_max*/
		if (cur_len < sizeof(pmsg->head)) {
			memcpy(&(pfifo->buffer[pfifo->wr]), &pmsg->head, cur_len);
			pfifo->wr = 0;
			memcpy(&(pfifo->buffer[pfifo->wr]), ((char *)&pmsg->head) + cur_len, sizeof(pmsg->head) - cur_len);
			pfifo->wr += sizeof(pmsg->head) - cur_len;
		} else if (cur_len == sizeof(pmsg->head)) {
			memcpy(&(pfifo->buffer[pfifo->wr]), &pmsg->head, sizeof(pmsg->head));
			pfifo->wr = 0;
		} else {
			memcpy(&(pfifo->buffer[pfifo->wr]), &pmsg->head, sizeof(pmsg->head));
			pfifo->wr += sizeof(pmsg->head);
		}
		/* step2: copy payload */
		cur_len = pfifo->depth_max - pfifo->wr; /* cur_len: wr to depth_max*/
		if (cur_len < pmsg->head.len) {
			memcpy(&(pfifo->buffer[pfifo->wr]), pmsg->payload, cur_len);
			pfifo->wr = 0;
			memcpy(&(pfifo->buffer[pfifo->wr]), pmsg->payload + cur_len, pmsg->head.len - cur_len);
			pfifo->wr += pmsg->head.len - cur_len;
		} else if (cur_len == pmsg->head.len) {
			memcpy(&(pfifo->buffer[pfifo->wr]), pmsg->payload, pmsg->head.len);
			pfifo->wr = 0;
		} else {
			memcpy(&(pfifo->buffer[pfifo->wr]), pmsg->payload, pmsg->head.len);
			pfifo->wr += pmsg->head.len;
		}
		pfifo->depth -= msg_total_len;
	}
	pthread_mutex_unlock(&pfifo->mutex);
	return 0;
}

int get_msg_fifo(link_msg_fifo *pfifo,  link_msg *pmsg)
{
	msg_head head = {0};
	unsigned int cur_len;

	if (pfifo == NULL || pmsg == NULL) {
		printf("get_msg_fifo  pointer null \n");
		return -1;
	}

	if (pmsg->payload == NULL) {
		printf("get_msg_fifo  payload null \n");
		return -1;
	}

	if (pfifo->depth == pfifo->depth_max) {
		printf("get_msg_fifo  empty \n");
		return -1; /* fifo empty */
	}
	pthread_mutex_lock(&pfifo->mutex);
	/* not aroud or fist time is equal*/
	if (pfifo->rd <= pfifo->wr) {
		memcpy(&pmsg->head, &(pfifo->buffer[pfifo->rd]), sizeof(pmsg->head));
		memcpy(pmsg->payload, &(pfifo->buffer[pfifo->rd + sizeof(pmsg->head)]), pmsg->head.len);
		pfifo->rd += sizeof(pmsg->head) + pmsg->head.len;
		pfifo->depth += sizeof(pmsg->head) + pmsg->head.len;
	} else {
		/* step1: copy head */
		cur_len = pfifo->depth_max - pfifo->rd; /* cur_len: rd to depth_max*/
		if (cur_len < sizeof(pmsg->head)) {
			memcpy(&pmsg->head, &(pfifo->buffer[pfifo->rd]), cur_len);
			pfifo->rd = 0;
			memcpy((((char *)&pmsg->head) + cur_len), &(pfifo->buffer[pfifo->rd]), sizeof(pmsg->head) - cur_len);
			pfifo->rd += sizeof(pmsg->head) - cur_len;
		} else if (cur_len == sizeof(pmsg->head)) {
			memcpy(&pmsg->head, &(pfifo->buffer[pfifo->rd]), sizeof(pmsg->head));
			pfifo->rd = 0;
		} else {
			memcpy(&pmsg->head, &(pfifo->buffer[pfifo->rd]), sizeof(pmsg->head));
			pfifo->rd += sizeof(pmsg->head);
		}
		/* step2: copy payload */
		cur_len = pfifo->depth_max - pfifo->rd; /* cur_len: wr to depth_max*/
		if (cur_len < pmsg->head.len) {
			memcpy(pmsg->payload, &(pfifo->buffer[pfifo->rd]), cur_len);
			pfifo->rd = 0;
			memcpy(pmsg->payload + cur_len, &(pfifo->buffer[pfifo->rd]), pmsg->head.len - cur_len);
			pfifo->rd += pmsg->head.len - cur_len;
		} else if (cur_len == pmsg->head.len) {
			memcpy(pmsg->payload, &(pfifo->buffer[pfifo->rd]), pmsg->head.len);
			pfifo->rd = 0;
		} else {
			memcpy(pmsg->payload, &(pfifo->buffer[pfifo->rd]), pmsg->head.len);
			pfifo->rd += pmsg->head.len;
		}
		pfifo->depth += sizeof(pmsg->head) + pmsg->head.len;
	}
	pthread_mutex_unlock(&pfifo->mutex);
	return 0;
}

link_msg_fifo *create_msg_fifo(size_t size)
{
	link_msg_fifo *msg_fifo = NULL;

	if (size > MSG_FIFO_MAX) {
		return -1;
	}

	msg_fifo = (link_msg_fifo *)malloc(sizeof(link_msg_fifo));
	if (msg_fifo == NULL) {
		return -1;
	}

	(void)memset(msg_fifo, 0, sizeof(msg_fifo));

	msg_fifo->buffer = (char *)malloc(size);
	if (msg_fifo->buffer == NULL) {
		return -1;
	}

	msg_fifo->depth = size;
	msg_fifo->depth_max = size;

	return msg_fifo;
}

void destroy_msg_fifo(link_msg_fifo *pfifo)
{
	free(pfifo->buffer);
	free(pfifo);
}


int send_single_message(int sockfd, link_msg *pmsg)
{
	struct msghdr msg = {0};
	msg_head head = {0};
	struct iovec io[2] = {0};

	if (pmsg == NULL) {
		return -1;
	}

	head.len = pmsg->head.len;
	head.type = pmsg->head.type;

	io[0].iov_base = &head;
	io[0].iov_len = sizeof(head);

	io[1].iov_base = pmsg->payload;
	io[1].iov_len = pmsg->head.len;

	msg.msg_iov = &io[0];
	msg.msg_iovlen = 2;
	msg.msg_control = 0;
	msg.msg_controllen = 0;

	if(sendmsg(sockfd, &msg, 0) < 0) {
		perror("send error.\n");
		return -1;
	}
	return 0;
}

//TCP是基于连接的串行数据流，你可以从连接的管道中，通过len控制每次从管道里面读取多少个字节的数据，
//没有读完的数据，不会丢弃，继续保留在管道中，TCP是天然保序的
//你可以首先读出一个固定长度的msg_head，msg_head中的len是用户协议约定好的payload数据的长度，所以读出了
//msg_head中的len，然后再从管道中读出msg_head.len的payload数据，就可以读出一个完整的可变长度的包来了
//所以下面的函数中用一个io就行了
int recv_single_message(int sockfd, link_msg *pmsg, unsigned int timeout)
{
	int ret;
	int flag;
	struct msghdr msg = {0};
	struct iovec io[1] = {0};
	struct timeval time;
	msg_head head = {0};

	if (pmsg == NULL)
	{
		perror("recv_message pmsg null\n");
		return -1;
	}

	if (timeout == 0) {
		flag = MSG_DONTWAIT;
	} else {
		flag = MSG_WAITALL;
		time.tv_sec = 0;
		time.tv_usec = timeout;
		if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&time, sizeof(time)) == -1) {
			perror("recv_message setsockopt:%08x  SO_RCVTIMEO error\n", sockfd);			
		}
	}

	io[0].iov_base = &head;
	io[0].iov_len = sizeof(head);
	msg.msg_iov = &io[0];
	msg.msg_iovlen = 1;
	msg.msg_control = 0;
	msg.msg_controllen = 0;
	ret = recvmsg(sockfd, &msg, flag);
	if(ret < 0) {
		//perror("recv error.\n");
		return -1;
	}

	io[0].iov_base = pmsg->payload;
	io[0].iov_len = head.len;
	msg.msg_iov = &io[0];
	msg.msg_iovlen = 1;
	msg.msg_control = 0;
	msg.msg_controllen = 0;
	ret = recvmsg(sockfd, &msg, flag);
	if(ret < 0) {
		//perror("recv error.\n");
		return -1;
	}

	pmsg->head.type = head.type;
	pmsg->head.len = head.len;

	return 0;
}

int link_send_single_message(link_msg *pmsg)
{
	int ret;
	ret = send_single_message(socket_dev.conn_fd, pmsg);
	return ret;
}

int link_recv_single_message(link_msg *pmsg, unsigned int timeout)
{
	int ret;
	//ret = recv_single_message(socket_dev.conn_fd, pmsg, timeout);
	ret = get_msg_fifo(socket_dev.fifo, pmsg);
	return ret;
}

int recv_single_message_for_task(link_msg *pmsg, unsigned int timeout)
{
	int ret;
	ret = recv_single_message(socket_dev.conn_fd, pmsg, timeout);
	return ret;
}

void link_close(void)
{
	close(socket_dev.conn_fd);
	if (socket_dev.listen_fd != 0) {
		close(socket_dev.listen_fd);	
	}
}

int link_get_info(unsigned int type, void *info)
{
	if (info == NULL) {
		return -1;
	}

	if (type == SYNC_LINK_INFO_STATUS) {
		return 	*((unsigned int *)info) = socket_dev.link_status;
	}

	return 0;
}

unsigned char data_buffer[PAYLOAD_MAX_LEN];
void *socket_recv_task()
{
	link_msg msg = {0};
 	int ret;
	unsigned int link_status = SYNC_LINK_DISCONNECTED;

	while (1) {
		link_get_info(SYNC_LINK_INFO_STATUS, &link_status);
		if (link_status == SYNC_LINK_DISCONNECTED) {
			usleep(500);
			continue;
		}
		msg.payload = data_buffer;
		ret = recv_single_message_for_task(&msg, 500);
		if (ret < 0) {
			continue;
		}
		ret = push_msg_fifo(socket_dev.fifo, &msg);
		if (ret < 0) {
			printf("push_msg_fifo failed ret:%d \n", ret);
		}
		usleep(500);
	}
}

int socket_init(unsigned int type);
void *client_entry();
void *server_entry();

socket_device socket_dev = {
	.listen_fd = 0,
	.conn_fd = 0,
	.link_status = SYNC_LINK_DISCONNECTED,
	.ops = {
		.init = socket_init,
		.send = link_send_single_message,
		.recv = link_recv_single_message,
		.get = link_get_info,
		.set = NULL,
		.close = link_close,
	},
	.fifo = 0,
};

int socket_init(unsigned int type)
{
	pthread_t tid;

	printf("socket_init...\n");

	socket_dev.link_status = SYNC_LINK_DISCONNECTED;

	socket_dev.fifo = create_msg_fifo(MSG_FIFO_LEN);
	if (socket_dev.fifo == NULL) {
		perror("create_msg_fifo error.\n");
		return -1;
	}

	pthread_mutex_init(&(socket_dev.fifo->mutex), NULL);

	if(pthread_create(&tid , NULL , socket_recv_task, 0) == -1)
	{
		perror("socket_recv_task pthread create error.\n");
		return -1;
	}

	if (type == DEVICE_TYPE_TCP_SERVER) {
		printf("server_init...\n");
		if(pthread_create(&tid , NULL , server_entry, 0) == -1)
		{
			perror("server_entry pthread create error.\n");
			return -1;
		}
	} else if (type == DEVICE_TYPE_TCP_CLIENT) {
		printf("client_init...\n");
		if(pthread_create(&tid , NULL , client_entry, 0) == -1)
		{
			perror("client_entry pthread create error.\n");
			return -1;
		}
	} else {
		perror(" socket_init error.\n");
		return -1;
	}
	return 0;
}

