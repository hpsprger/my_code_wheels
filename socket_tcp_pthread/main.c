#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "data_trans.h"



char send_text[PAYLOAD_MAX_LEN];
char rcv_text[PAYLOAD_MAX_LEN];

char server_ip[PAYLOAD_MAX_LEN];

#if 0
// ./chnnl_sync 0 ==> server
// ./chnnl_sync 1 127.0.0.1 ==> client 连接服务器，并通信
void main(int argc, char* argv[]) 
{
	char choose[CHOOSE_MAX_LINE];
	pthread_t tid;
	link_msg tx_msg = {0};
	link_msg rx_msg = {0};	
	int ret;

    if(argc < 2)
    {
        printf("usage: ./main 1 192.168.8.2\n");
        exit(1);
    }

    printf("main --> argv[0]=%s  argv[1]=%d  argc=%d \n", argv[0], atoi(argv[1]), argc);

	memset(server_ip, 0, sizeof(server_ip));
	if (argc >= 3) {
		memcpy(server_ip, argv[2], strlen(argv[2])+1);
		printf("server addr:%s \n", server_ip);
	}

	data_trans_init(atoi(argv[1]));

	while (1) {
# if 0
		printf("pls choose: 1.send 2.recv 3.exit \n");
		memset(choose , 0 , CHOOSE_MAX_LINE);
		if ((fgets(choose , CHOOSE_MAX_LINE , stdin)) != NULL) {
			if(strcmp(choose , "1\n") == 0)
			{
				printf("input msg: \n");
				memset(send_text , 0 , PAYLOAD_MAX_LEN);
				fgets(send_text , PAYLOAD_MAX_LEN , stdin);
				tx_msg.head.type = 0x1234;
				tx_msg.head.len = strlen(send_text);
				tx_msg.payload = send_text;
				if (data_trans_send_msg(&tx_msg) < 0) {
					printf("send_message fail \n");
				} else {
					printf("send_message ok \n");
				}
			}
			if(strcmp(choose , "2\n") == 0)
			{
				memset(rcv_text , 0 , PAYLOAD_MAX_LEN);
				rx_msg.payload = rcv_text;
				ret = data_trans_recv_single_msg(&rx_msg, 1000);
				printf("recv:%s \n", (ret == 0)?"ok":"fail");
				printf("rx msg:type(0x%x) \n", rx_msg.head.type);
				printf("rx msg:len(%d) \n", rx_msg.head.len);
				printf("rx msg:buffer==>%s\n", rx_msg.payload);
			}
			if(strcmp(choose , "3\n") == 0)
			{
				printf("data_trans_close...\n");
				data_trans_close();
				exit(1);
			}
		}
#endif
		sleep(1);
	}
	return 0;
}
#endif


link_msg_fifo_without_lock *pfifo = NULL;

#define SHM_NAME "fifo_unlock_test"
#define SHM_SIZE 0x400000
#define FIFO_UNLOCK_SIZE 1024   //fifo size must be 2'n 
#define MAGIC_NUM 0xa5a5
#define BUFFER1 0x11
#define BUFFER2 0x22

unsigned char tx_msg_buffer[4096];
unsigned char rx_msg_buffer[4096];

void * test_fifo_thread_tx()
{
	unsigned int delay = 0;
	unsigned int len = 0;
	unsigned int count = 0;
	unsigned int count_tx_ok = 0;
	unsigned int count_tx_fail = 0;
	int fd;
	link_msg msg = {0};
	int ret =0;

	fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
	if (fd == -1) {
		perror("pthread shm_open error.\n");
		return -1;
	}

	ftruncate(fd, SHM_SIZE);

	pfifo = (link_msg_fifo_without_lock *)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (pfifo == NULL) {
		perror("pthread mmap error.\n");
		return -1;
	}

	printf("fifo_without_lock size:0x%d \n", FIFO_UNLOCK_SIZE);

	(void)memset(pfifo, 0, SHM_SIZE);

	pfifo->size = FIFO_UNLOCK_SIZE; //fifo size must be 2'n 

	while (1) {
		delay = rand()%100;
		len = rand()%10 * 20;
		msg.head.type = MAGIC_NUM;
		msg.head.len = len + 4;//因为有buffer[0] buffer[1]的判断，所以至少要有4个字节
		msg.payload = tx_msg_buffer;
		tx_msg_buffer[0] = BUFFER1;
		tx_msg_buffer[1] = BUFFER2;
		count++;
		ret = push_msg_fifo_without_lock(pfifo, &msg);
		if (ret != 0) {
			count_tx_fail++;
			printf("push_msg_fifo_without_lock send fail count:%d  count_tx_ok:%d count_tx_fail:%d ", count, count_tx_ok, count_tx_fail);
		} else {
			count_tx_ok++;
			printf("push_msg_fifo_without_lock send ok count:%d  count_tx_ok:%d count_tx_fail:%d ", count, count_tx_ok, count_tx_fail);
		}
		if (delay > 70) {
			delay = 800;
		}
		delay = delay * 500;
		printf("usleep:%d ========= \n\n\n", delay);
		usleep(delay);
	}

	munmap(pfifo, SHM_SIZE);

	return 0;
}

void * test_fifo_thread_rx()
{
	unsigned int delay = 0;
	unsigned int len = 0;
	int fd;
	link_msg msg = {0};
	int ret =0;
	unsigned int count = 0;
	unsigned int check_err_count = 0;
	unsigned int count_rx_ok = 0;
	unsigned int count_rx_fail = 0;
	unsigned int check_pass_count = 0;
	fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
	if (fd == -1) {
		perror("pthread shm_open error.\n");
		return -1;
	}

	ftruncate(fd, SHM_SIZE);

	pfifo = (link_msg_fifo_without_lock *)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (pfifo == NULL) {
		perror("pthread mmap error.\n");
		return -1;
	}

	printf("fifo_without_lock size:0x%d \n", FIFO_UNLOCK_SIZE);

	(void)memset(pfifo, 0, SHM_SIZE);

	pfifo->size = FIFO_UNLOCK_SIZE; //fifo size must be 2'n 

	while (1) {
		delay = rand()%100;
		msg.head.type = 0;
		msg.head.len = 0;
		msg.payload = rx_msg_buffer;
		rx_msg_buffer[0] = 0;
		rx_msg_buffer[1] = 0;		
		if (delay > 70) {
			delay = 800;
		}
		delay = delay * 500;
		count++;
		ret = get_msg_fifo_without_lock(pfifo, &msg);
		if (ret != 0) {
			count_rx_fail++;
			printf("test_fifo_thread_rx  recv-fail count:%d count_rx_ok:%d count_rx_fail:%d check_pass_count:%d  check_err_count:%d  usleep:%d  ========\n\n\n", count, count_rx_ok, count_rx_fail, check_pass_count, check_err_count, delay);
			usleep(delay);
			continue;
		}
		count_rx_ok++;
		if ((msg.head.type != MAGIC_NUM) || (rx_msg_buffer[0] != BUFFER1) || (rx_msg_buffer[1] != BUFFER2)){
			check_err_count++;
			printf("test_fifo_thread_rx check-error count:%d count_rx_ok:%d count_rx_fail:%d check_pass_count:%d check_err_count:%d  type:0x%x data[0]:0x%x data[1]:0x%x usleep:%d ========\n\n\n ", count, count_rx_ok, count_rx_fail, check_pass_count, check_err_count, msg.head.type, rx_msg_buffer[0], rx_msg_buffer[1], delay);
		} else {
			check_pass_count++;
			printf("test_fifo_thread_rx check-pass  count:%d count_rx_ok:%d count_rx_fail:%d check_pass_count:%d check_err_count:%d  type:0x%x data[0]:0x%x data[1]:0x%x usleep:%d ========\n\n\n ", count, count_rx_ok, count_rx_fail, check_pass_count, check_err_count, msg.head.type, rx_msg_buffer[0], rx_msg_buffer[1], delay);
		}
		usleep(delay);
	}

	munmap(pfifo, SHM_SIZE);

	return 0;
}


// 这个函数用来测试 无锁的 一个生产者 一个消费者 的 fifo 的使用  
// ./chnnl_sync 0 ==> tx
// ./chnnl_sync 1 ==> rx 
void main(int argc, char* argv[]) 
{
	pthread_t tid;

	srand((unsigned)time(NULL)); 

	if (atoi(argv[1]) == 0x0) {
		if(pthread_create(&tid , NULL , test_fifo_thread_tx, 0) == -1) {
			perror("pthread test_fifo_thread_tx error.\n");
			return -1;
		}
	} else {
		if(pthread_create(&tid, NULL , test_fifo_thread_rx, 0) == -1) {
			perror("pthread test_fifo_thread_rx error.\n");
			return -1;
		}
	}

	while (1) {
		sleep(1);
	}

	return 0;
}
