#include "phy_eth_common.h"

// p *socket_dev.fifo
// x /32x socket_dev.fifo->buffer
//我这个带锁的设计，适合用于多个生产者，多个消费者，但是不适用于 PCIE RC EP 之间的这种跨系统的共享内存中使用
//如果是多个生产者，多个消费者，必须要用hwspinlock了，peteson锁只能用于一个生产者 一个消费者的情况
int push_msg_fifo(link_msg_fifo *pfifo,  link_msg *pmsg)
{
	unsigned int msg_total_len;
	unsigned int cur_len;
	unsigned int i ;
	if (pfifo == NULL || pmsg == NULL) {
		printf("push_msg_fifo pointer null\n");
		return -1;
	}

	if (pmsg->payload == NULL) {
		printf("push_msg_fifo payload null\n");
		return -1;
	}
	
	pthread_mutex_lock(&pfifo->mutex);

	msg_total_len = sizeof(pmsg->head) + pmsg->head.len;

	if (pfifo->depth < msg_total_len) {
		printf("push_msg_fifo  free space not enough \n");
		pthread_mutex_unlock(&pfifo->mutex);
		return -1; /* free space not enough */
	}
	//printf("pfifo->wr:%d\n", pfifo->wr);
	/* aroud or fist time is equal*/
	if (pfifo->wr < pfifo->rd) {
		memcpy(&(pfifo->buffer[pfifo->wr]), &pmsg->head, sizeof(pmsg->head));
		//printf("000 -- ");
		//for (i = 0; i < sizeof(pmsg->head); i++) {
		//	printf("0x%x ", pfifo->buffer[pfifo->wr + i]);
		//}
		//printf("\n");
		memcpy(&(pfifo->buffer[pfifo->wr + sizeof(pmsg->head)]), pmsg->payload, pmsg->head.len);
		pfifo->wr += msg_total_len;
		pfifo->depth -= msg_total_len;
		pfifo->wr %= pfifo->depth_max;
	} else {
		/* step1: copy head */
		cur_len = pfifo->depth_max - pfifo->wr; /* cur_len: wr to depth_max*/
		if (cur_len < sizeof(pmsg->head)) {
			memcpy(&(pfifo->buffer[pfifo->wr]), &pmsg->head, cur_len);
			//for (i = 0; i < cur_len; i++) {
			//	printf("111 0x%x ", pfifo->buffer[pfifo->wr + i]);
			//}
			//printf("\n");
			pfifo->wr = 0;
			memcpy(&(pfifo->buffer[pfifo->wr]), ((char *)&pmsg->head) + cur_len, sizeof(pmsg->head) - cur_len);
			//for (i = 0; i < (sizeof(pmsg->head) - cur_len); i++) {
			//	printf("111 0x%x ", pfifo->buffer[pfifo->wr + i]);
			//}
			//printf("\n");			
			pfifo->wr += sizeof(pmsg->head) - cur_len;
		} else if (cur_len == sizeof(pmsg->head)) {
			memcpy(&(pfifo->buffer[pfifo->wr]), &pmsg->head, sizeof(pmsg->head));
			//for (i = 0; i < sizeof(pmsg->head); i++) {
			//	printf("222 0x%x ", pfifo->buffer[pfifo->wr + i]);
			//}
			//printf("\n");
			pfifo->wr = 0;
		} else {
			memcpy(&(pfifo->buffer[pfifo->wr]), &pmsg->head, sizeof(pmsg->head));
			//for (i = 0; i < sizeof(pmsg->head); i++) {
			//	printf("333 0x%x ", pfifo->buffer[pfifo->wr + i]);
			//}
			//printf("\n");
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
		pfifo->wr %= pfifo->depth_max;
	}

	pthread_mutex_unlock(&pfifo->mutex);
	return 0;
}

// 断点调试   watch socket_dev.fifo->depth if socket_dev.fifo->depth > 128 
// 断点调试   watch socket_dev.fifo->depth   p *socket_dev.fifo
// 这个fifo也可以做成像 without lock 这种无锁设计的fifo一样，size 调整为2的n次方，就不用%操作了
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

	pthread_mutex_lock(&pfifo->mutex);

	if (pfifo->depth == pfifo->depth_max) {
		//printf("get_msg_fifo  empty \n");
		pthread_mutex_unlock(&pfifo->mutex);
		return -1; /* fifo empty */
	}
	//printf("pfifo->rd:%d\n", pfifo->rd);
	/* not aroud or fist time is equal*/
	if (pfifo->rd < pfifo->wr) {
		memcpy(&pmsg->head, &(pfifo->buffer[pfifo->rd]), sizeof(pmsg->head));
		memcpy(pmsg->payload, &(pfifo->buffer[pfifo->rd + sizeof(pmsg->head)]), pmsg->head.len);
		pfifo->rd += sizeof(pmsg->head) + pmsg->head.len;
		pfifo->depth += sizeof(pmsg->head) + pmsg->head.len;
		pfifo->rd %= pfifo->depth_max;
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
		pfifo->rd %= pfifo->depth_max;
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

	msg_fifo->size = size;
	msg_fifo->depth = size;
	msg_fifo->depth_max = size;

	return msg_fifo;
}

void destroy_msg_fifo(link_msg_fifo *pfifo)
{
	free(pfifo->buffer);
	free(pfifo);
}

//针对一个生产者  一个消费者的情况(多个肯定要用锁)，通过让生产者来只更新wr指针，让消费者只更新rd指针，来做到无锁的FIFO设计
//如果是多个生产者，多个消费者，如果是在一个系统中,可以用系统的锁来互斥
//如果在多个系统间，只有一个生成者与一个消费者，可以用FIFO 这种无锁的设计，比如PCIE 的 RC  EP 要操作同一片共享内存做的FIFO的时候，可以用下面这种无锁设计，比如仅有的一个生产者RC去更新WR指针，仅有的一个消费者EP去更新RD指针，如果有多个消费者与生产者这又是不行的
//如果像PCIE RC EP 这种跨系统的，假设又有多个生产者与消费者，那么就必须使用锁了，但是如果有某一个系统中锁显然是行不通的，所以只能用 hwspinlock锁 （peteson锁 能实现纯软件的锁实现，但是也只能是一个消费者 一个生产者的情况）
//上面说的这种一个消费者 一个生产者的无锁FIFO设计，有人会说，你的push 或 get 函数中一定会用到 wr rd指针，这难道不会有竞态么？？？
//我想说，竞态肯定有，比如push函数中，你去读rd指针，读到的rd值，有可能是没有更新的rd值，也有可能是更新后的rd值，但都不会影响FIFO的正常功能，比如你读的是老的rd值，那你的wr指针最多也就停留在老的这个rd位置，如果这时rd其实需要更新了，也没关系，比如多一次空判断，如果读到的就是更新后的rd值那就是正常的情况了，那正好符号最新的情况，最好了
//所以wr也是类似的分析，所以也就是说这两个可能存在的竞争中间态，对FIFO功能无影响
//FIFO 的 wr rd 指针，可以这么设计，来避免通过 %运算来取余的操作
//1.FIFO的wr rd 用来做 写 读 数据的总计数器，一直往上加，做实际 写 读 的数据的总计数，计数的最大值回绕也没有关系 
//2.FIFO的buffer的大小一定要为2的n次方的大小 ==> 这样 buffer的 wr_index or rd_index 的值 就是 wr or rd 总计数值 的 n-1:0 bit, 这样就避免了取余操作了
//    wr_index = wr[n-1:0]
//    rd_index = rd[n-1:0]
//  wr == rd ==> FIFO 为空
//  fifo_free_space = fifo_size  - (wr - rd)
unsigned int fifo_full_cnt = 0;
unsigned int fifo_empty_cnt = 0;
int push_msg_fifo_without_lock(link_msg_fifo_without_lock *pfifo,  link_msg *pmsg)
{
	unsigned int msg_total_len;
	unsigned short free_space;
	unsigned int cur_len;
	unsigned int i;
	unsigned int wr;
	unsigned int rd;

	if (pfifo == NULL || pmsg == NULL) {
		printf("push_msg_fifo pointer null\n");
		return -1;
	}

	if (pmsg->payload == NULL) {
		printf("push_msg_fifo payload null\n");
		return -1;
	}

	msg_total_len = sizeof(pmsg->head) + pmsg->head.len;

	free_space = pfifo->size - (unsigned short)(pfifo->wr - pfifo->rd); //注意这里一定要强转一下，否则相减后结果就是一个很大的值，出现异常，正式代码中这里注意是unsigned int 类型，unsigned short 为测试方便，free_space 也要用对应的类型 unsigned short

	if (free_space < msg_total_len) {
		fifo_full_cnt++;
		printf("space is not enough ......wr:%d rd:%d  msg_total_len:0x%x free_space:0x%x size:0x%x fifo_full_cnt:%d \n", pfifo->wr, pfifo->rd, msg_total_len, free_space, pfifo->size, fifo_full_cnt);
		return -1;
	} else {
		printf("space is     enough ......wr:%d rd:%d msg_total_len:0x%x free_space:0x%x size:0x%x fifo_full_cnt:%d \n", pfifo->wr, pfifo->rd, msg_total_len, free_space, pfifo->size, fifo_full_cnt);
	}

	wr = GET_WR_INDEX(pfifo);
	rd = GET_RD_INDEX(pfifo);


	/* aroud or fist time is equal*/
	if (wr < rd) {
		memcpy(&(pfifo->buffer[wr]), &pmsg->head, sizeof(pmsg->head));
		memcpy(&(pfifo->buffer[wr + sizeof(pmsg->head)]), pmsg->payload, pmsg->head.len);
	} else {
		/* step1: copy head */
		cur_len = pfifo->size - wr; /* cur_len: wr to depth_max*/
		if (cur_len < sizeof(pmsg->head)) {
			memcpy(&(pfifo->buffer[wr]), &pmsg->head, cur_len);
			wr = 0;
			memcpy(&(pfifo->buffer[wr]), ((char *)&pmsg->head) + cur_len, sizeof(pmsg->head) - cur_len);		
			wr += sizeof(pmsg->head) - cur_len;
		} else if (cur_len == sizeof(pmsg->head)) {
			memcpy(&(pfifo->buffer[wr]), &pmsg->head, sizeof(pmsg->head));
			wr = 0;
		} else {
			memcpy(&(pfifo->buffer[wr]), &pmsg->head, sizeof(pmsg->head));
			wr += sizeof(pmsg->head);
		}
		/* step2: copy payload */
		cur_len = pfifo->size - wr; /* cur_len: wr to depth_max*/
		if (cur_len < pmsg->head.len) {
			memcpy(&(pfifo->buffer[wr]), pmsg->payload, cur_len);
			wr = 0;
			memcpy(&(pfifo->buffer[wr]), pmsg->payload + cur_len, pmsg->head.len - cur_len);
			wr += pmsg->head.len - cur_len;
		} else if (cur_len == pmsg->head.len) {
			memcpy(&(pfifo->buffer[wr]), pmsg->payload, pmsg->head.len);
			wr = 0;
		} else {
			memcpy(&(pfifo->buffer[wr]), pmsg->payload, pmsg->head.len);
			wr += pmsg->head.len;
		}
	}

	pfifo->wr += msg_total_len;

	return 0;
}

int get_msg_fifo_without_lock(link_msg_fifo_without_lock *pfifo,  link_msg *pmsg)
{
	msg_head head = {0};
	unsigned int cur_len;
	unsigned int wr;
	unsigned int rd;
	
	if (pfifo == NULL || pmsg == NULL) {
		printf("get_msg_fifo  pointer null \n");
		return -1;
	}

	if (pmsg->payload == NULL) {
		printf("get_msg_fifo  payload null \n");
		return -1;
	}

	if (pfifo->wr == pfifo->rd) {
		fifo_empty_cnt++;
		printf("get_msg_fifo  fifo empty  ..... wr:%d rd:%d size:0x%x fifo_empty_cnt:%d \n", pfifo->wr, pfifo->rd, pfifo->size, fifo_empty_cnt);
		return -1;
	} else {
		printf("get_msg_fifo  fifo normal ..... wr:%d rd:%d size:0x%x fifo_empty_cnt:%d \n", pfifo->wr, pfifo->rd, pfifo->size, fifo_empty_cnt);
	}

	wr = GET_WR_INDEX(pfifo);
	rd = GET_RD_INDEX(pfifo);

	if (rd < wr) {
		memcpy(&pmsg->head, &(pfifo->buffer[rd]), sizeof(pmsg->head));
		memcpy(pmsg->payload, &(pfifo->buffer[rd + sizeof(pmsg->head)]), pmsg->head.len);
	} else {
		/* step1: copy head */
		cur_len = pfifo->size - rd; /* cur_len: rd to depth_max*/
		if (cur_len < sizeof(pmsg->head)) {
			memcpy(&pmsg->head, &(pfifo->buffer[rd]), cur_len);
			rd = 0; 
			memcpy((((char *)&pmsg->head) + cur_len), &(pfifo->buffer[rd]), sizeof(pmsg->head) - cur_len);
			rd += sizeof(pmsg->head) - cur_len;
		} else if (cur_len == sizeof(pmsg->head)) {
			memcpy(&pmsg->head, &(pfifo->buffer[rd]), sizeof(pmsg->head));
			rd = 0;
		} else {
			memcpy(&pmsg->head, &(pfifo->buffer[rd]), sizeof(pmsg->head));
			rd += sizeof(pmsg->head);
		}
		/* step2: copy payload */
		cur_len = pfifo->size - rd; /* cur_len: wr to depth_max*/
		if (cur_len < pmsg->head.len) {
			memcpy(pmsg->payload, &(pfifo->buffer[rd]), cur_len);
			rd = 0;
			memcpy(pmsg->payload + cur_len, &(pfifo->buffer[rd]), pmsg->head.len - cur_len);
			rd += pmsg->head.len - cur_len;
		} else if (cur_len == pmsg->head.len) {
			memcpy(pmsg->payload, &(pfifo->buffer[rd]), pmsg->head.len);
			rd = 0;
		} else {
			memcpy(pmsg->payload, &(pfifo->buffer[rd]), pmsg->head.len);
			rd += pmsg->head.len;
		}
	}

	pfifo->rd += sizeof(pmsg->head) + pmsg->head.len;

	return 0;
}

unsigned int get_first_bit_1(unsigned int size) 
{
	unsigned int  i = 0;
	unsigned int  shift_data = (unsigned int)0x8000000;
	for (i = 0; i < 32; i++) {
		if (shift_data & size) {
			return 31 - i;
		} else {
			shift_data >>= 1;
		}
	}
	return 0;
}

unsigned int get_adjusted_size_2n(unsigned int size) 
{
	int i = get_first_bit_1(size);
	if (((unsigned int)0x1 << i) < size) {
		return ((unsigned int)0x1 << (i + 1));
	} else {
		return ((unsigned int)0x1 << i);
	}
}

link_msg_fifo_without_lock *create_msg_fifo_without_lock(size_t size)
{
	link_msg_fifo_without_lock *msg_fifo = NULL;
	unsigned int adjuct_size = get_adjusted_size_2n(size);

	if (adjuct_size > MSG_FIFO_MAX) {
		return -1;
	}

	printf("fifo_without_lock size:0x%d \n", adjuct_size);

	msg_fifo = (link_msg_fifo_without_lock *)malloc(sizeof(link_msg_fifo_without_lock) + adjuct_size);
	if (msg_fifo == NULL) {
		return -1;
	}

	(void)memset(msg_fifo, 0, sizeof(msg_fifo));

	msg_fifo->size = adjuct_size;

	return msg_fifo;
}

void destroy_msg_fifo_without_lock(link_msg_fifo_without_lock *pfifo)
{
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
	//ret = get_msg_fifo(socket_dev.fifo, pmsg); //有锁的设计
	ret = get_msg_fifo_without_lock(socket_dev.fifo_without_lock, pmsg); //无锁的设计
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
		//printf("push_msg_fifo   msg.head.len:%d \n", msg.head.len);
		//ret = push_msg_fifo(socket_dev.fifo, &msg); //使用带锁的设计
		ret = push_msg_fifo_without_lock(socket_dev.fifo_without_lock, &msg);//使用无锁设计
		if (ret < 0) {
			printf("push_msg_fifo failed ret:%d \n", ret);
		}
		usleep(100);
	}
}

int socket_init(unsigned int type)
{
	pthread_t tid;

	printf("socket_init...\n");

	socket_dev.link_status = SYNC_LINK_DISCONNECTED;

#if 0
	socket_dev.fifo = create_msg_fifo(MSG_FIFO_LEN);
	if (socket_dev.fifo == NULL) {
		perror("create_msg_fifo error.\n");
		return -1;
	}

	pthread_mutex_init(&(socket_dev.fifo->mutex), NULL);

#endif 

	socket_dev.fifo_without_lock = create_msg_fifo(MSG_FIFO_LEN); 
	if (socket_dev.fifo_without_lock == NULL) {
		perror("create_msg_fifo error.\n");
		return -1;
	}

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
	.fifo_without_lock = 0,
};
