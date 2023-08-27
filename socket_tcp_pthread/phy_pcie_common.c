#include "phy_eth_common.h"

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


typedef struct _msg_head {
	unsigned short type;
	unsigned short len;
} msg_head;

typedef struct _link_msg {
	msg_head head;
	unsigned char  *payload;
} link_msg;

#define LINK_MSG_FIFO_DEPT 1024

typedef struct _link_msg_fifo {
	unsigned int wr;//指向起始写位置
	unsigned int rd;//指向起始读位置
	unsigned int flag;
	char buffer[LINK_MSG_FIFO_DEPT];
} link_msg_fifo;

link_msg_fifo msg_fifo;

int msg_push_fifo(fifo pfifo,  link_msg *pmsg)
{
	unsigned int cur_free_len;
	if (pfifo->rd == pfifo->wr) {
		cur_free_len = LINK_MSG_FIFO_DEPT;
	} else if (pfifo->rd > pfifo->wr) {
		cur_free_len = pfifo->wr + LINK_MSG_FIFO_DEPT - pfifo->rd;
	} else {
		cur_free_len = pfifo->wr - pfifo->rd;
	}

	if (cur_free_len < (pmsg->head.len + sizeof(msg_head))) {
		return -1;//free space is not enough 
	}


	
	if (LINK_MSG_FIFO_DEPT - (pfifo->wr - pfifo->rd)) 
	memcpy(pfifo->buffer[pfifo->wr], pmsg->head, sizeof(msg_head));
	memcpy(pfifo->buffer[pfifo->wr], pmsg->payload, pmsg->head.len);	
}
 
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
		perror("recv error.\n");
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
		perror("recv error.\n");
		return -1;
	}

	pmsg->head.type = head.type;
	pmsg->head.len = head.len;

	return 0;
}