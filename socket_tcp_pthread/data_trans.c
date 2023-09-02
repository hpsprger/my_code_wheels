#include "data_trans.h"

device_ops *data_trans_dev_ops[] = {
	&socket_dev.ops,
	&socket_dev.ops,
	//
};

data_trans_device data_trans;

int get_link_info(unsigned int *link_status)
{
	int ret;
	ret = data_trans.ops->get(SYNC_LINK_INFO_STATUS, link_status);
	return ret;
}

unsigned char send_buffer[4096];

int data_trans_send_single_msg(unsigned int msg_type)
{
	int ret;
	int msg_len = 0;
	link_msg msg;
	if (msg_type >= SYNC_MSG_MAX) {
		return -1;
	}

	//这里进行变长的报文测试
	msg_len = rand()%20 * 5;
	printf("data_trans_send_single_msg msglen=%d \n", msg_len);
	msg.head.type = msg_type;
	msg.head.len = msg_len;
	msg.payload = send_buffer;
	ret = data_trans.ops->send(&msg);
	return ret;
}

int data_trans_send_msg(link_msg *pmsg)
{
	int ret;
	if (pmsg == NULL) {
		return -1;
	}
	ret = data_trans.ops->send(pmsg);
	return ret;
}

void data_trans_close(void)
{
	int ret;
	ret = data_trans.ops->close();
	return ret;
}


int data_trans_recv_single_msg(link_msg *pmsg, unsigned int timeout)
{
	int ret;
	ret = data_trans.ops->recv(pmsg, timeout);
	return ret;
}

unsigned int task_count;

void * sync_fsm_translation()
{
	int ret;
	unsigned int delay;

	unsigned int one_task_fsm_change_count = 0;
	unsigned int one_task_fsm_change_count_min = 0xffffffff;
	unsigned int one_task_fsm_change_count_max = 0;
	unsigned long long one_task_fsm_change_count_sum = 0;
	float one_task_fsm_change_count_avg = 0.0;

	unsigned int err_count = 0;
	unsigned int err_count_max = 0x0;
	unsigned int err_count_min = 0xffffffff;
	link_msg msg = {0};
	unsigned int link_fsm = SYNC_LINK_SETUP;
	unsigned int link_status = SYNC_LINK_DISCONNECTED;

	unsigned char rx_buffer[PAYLOAD_MAX_LEN];
	struct timeval time = { 
			.tv_sec=0, /*单位：s*/
			.tv_usec=0 /*单位：ns*/
	};
	struct timespec ts = {
		.tv_sec = 0,  /* 秒*/
		.tv_nsec = 0, /* 纳秒*/
	};
	long long milliseconds = 0;
	long long last_milliseconds = 0;
	long long count_per_second = 0;
	unsigned long long int sec,usec = 0;

	NORMAL_PRINTF(" =========fsm_translation======= \n");

	while (1) {
		one_task_fsm_change_count++;
		switch(link_fsm) {
		case SYNC_LINK_SETUP:
		get_link_info(&link_status);
		if (link_status == SYNC_LINK_CONNECTED) {
			NORMAL_PRINTF("SYNC_LINK_SETUP =========0======= pass\n");
			link_fsm = SYNC_LINK_START_TX;
			one_task_fsm_change_count = 0;
		} else {
			NORMAL_PRINTF("SYNC_LINK_SETUP =========0======= nopass SYNC_LINK_DISCONNECTED\n");
		}
		break;

		case SYNC_LINK_START_TX:
		ret = data_trans_send_single_msg(SYNC_MSG_START);
		if (ret < 0) {
			ERROR_PRRINT("SYNC_LINK_START_TX =========1======= no pass send fail err_count=%d \n", err_count);
			err_count++;
			break;
		} else {
			NORMAL_PRINTF("SYNC_LINK_START_TX =========1======= pass \n");
			link_fsm = SYNC_LINK_START_RX;
			err_count = 0;
		}
		break;

		case SYNC_LINK_START_RX:
		msg.payload = rx_buffer;
		ret = data_trans_recv_single_msg(&msg, RX_TIMEOUT);
		if (ret < 0) {
			//ERROR_PRRINT("SYNC_LINK_START_RX =========2======= nopass recv fail err_count=%d \n", err_count);
			err_count++;
			usleep(RX_TIMEOUT);
			break;
		}

		if (msg.head.type == SYNC_MSG_START) {
			NORMAL_PRINTF("SYNC_LINK_START_RX =========2======= pass \n");
			link_fsm = SYNC_LINK_HIGH_TX;
			err_count = 0;
		} else {
			ERROR_PRRINT("SYNC_LINK_START_RX =========2======= nopass type mismatching err_count=%d \n", err_count);
			err_count++;
		}
		break;

		case SYNC_LINK_HIGH_TX:
		ret = data_trans_send_single_msg(SYNC_MSG_HIGH);
		if (ret < 0) {
			ERROR_PRRINT("SYNC_LINK_HIGH_TX =========3======= nopass send fail err_count=%d \n", err_count);
			err_count++;
			break;
		} else {
			NORMAL_PRINTF("SYNC_LINK_HIGH_TX =========3======= pass \n");
			link_fsm = SYNC_LINK_HIGH_RX;
			err_count = 0;
		}
		break;
	
		case SYNC_LINK_HIGH_RX:
		msg.payload = rx_buffer;
		ret = data_trans_recv_single_msg(&msg, RX_TIMEOUT);
		if (ret < 0) {
			//ERROR_PRRINT("SYNC_LINK_HIGH_RX =========4======= nopass recv fail err_count=%d \n", err_count);
			err_count++;
			usleep(RX_TIMEOUT);
			break;
		}

		if (msg.head.type == SYNC_MSG_HIGH) {
			NORMAL_PRINTF("SYNC_LINK_HIGH_RX =========4======= pass \n");
			link_fsm = SYNC_LINK_LOW_TX;
			err_count = 0;
		} else {
			ERROR_PRRINT("SYNC_LINK_HIGH_RX =========4======= nopass type mismatching err_count=%d \n", err_count);
			err_count++;
		}
		break;


		case SYNC_LINK_LOW_TX:
		ret = data_trans_send_single_msg(SYNC_MSG_LOW);
		if (ret < 0) {
			ERROR_PRRINT("SYNC_LINK_LOW_TX =========5======= nopass send fail err_count=%d\n", err_count);
			err_count++;
			break;
		} else {
			NORMAL_PRINTF("SYNC_LINK_LOW_TX =========5======= pass \n");
			link_fsm = SYNC_LINK_LOW_RX;
			err_count = 0;
		}
		break;

		case SYNC_LINK_LOW_RX:
		msg.payload = rx_buffer;
		ret = data_trans_recv_single_msg(&msg, RX_TIMEOUT);
		if (ret < 0) {
			//ERROR_PRRINT("SYNC_LINK_LOW_RX =========6======= nopass recv fail err_count=%d \n", err_count);
			err_count++;
			usleep(RX_TIMEOUT);
			break;
		}

		if (msg.head.type == SYNC_MSG_LOW) {
			NORMAL_PRINTF("SYNC_LINK_LOW_RX =========6======= pass \n");
			link_fsm = SYNC_LINK_TASKING;
			err_count = 0;
		} else {
			ERROR_PRRINT("SYNC_LINK_LOW_RX =========6======= nopass type mismatching err_count=%d \n", err_count);
			err_count++;
		}
		break;

		case SYNC_LINK_TASKING:
		delay = rand()%10 * 150;
#if 0
		clock_gettime(0, &time);
		sec = (unsigned long long int)time.tv_sec;
		usec = (unsigned long long int)time.tv_usec;
		NORMAL_PRINTF("SYNC_LINK_TASKING =====7===delay:%d====task_count=%d(time:0x%lld)===== \n", delay, task_count++, sec*1000000 + usec/1000);
		NORMAL_PRINTF("SYNC_LINK_TASKING =====7===delay:%dus===task_count=%d==time:sec:%lld--usec:%lld===== \n", delay, task_count++, sec, usec);
#endif 
		(void)clock_gettime(CLOCK_REALTIME, &ts);
		milliseconds = (ts.tv_sec*1000) + (ts.tv_nsec/1000000);
		NORMAL_PRINTF("SYNC_LINK_TASKING =====7===delay:%dus===task_count=%d==milliseconds:%lld===== \n", delay, task_count, milliseconds);
		usleep(delay);
		NORMAL_PRINTF("SYNC_LINK_TASKING ========7==done======= \n");

		one_task_fsm_change_count_sum += one_task_fsm_change_count;
		if (one_task_fsm_change_count > one_task_fsm_change_count_max) {
			one_task_fsm_change_count_max = one_task_fsm_change_count;
		}
		if (one_task_fsm_change_count < one_task_fsm_change_count_min) {
			one_task_fsm_change_count_min = one_task_fsm_change_count;
		}
		one_task_fsm_change_count_avg = (float)one_task_fsm_change_count_sum / (float)(task_count+1);

		link_fsm = SYNC_LINK_START_TX;
		err_count = 0;

		if (task_count % 5000 == 0) {
			(void)clock_gettime(CLOCK_REALTIME, &ts);
			milliseconds = (ts.tv_sec*1000) + (ts.tv_nsec/1000000);
			if (last_milliseconds == 0) {
				last_milliseconds = milliseconds;
				count_per_second = 0;
			} else {
				//count_per_second = ((float)5000 / (float)(milliseconds - last_milliseconds - 5000)) * (float)1000.0;
				count_per_second = ((float)5000 / (float)(milliseconds - last_milliseconds)) * (float)1000.0;
				last_milliseconds = milliseconds;
			}

			ERROR_PRRINT("SYNC_LINK_TASKING ======dly:%dus===tsk_cnt=%d===ms:%lld===tskcnt_per_sec=%lld===err_cnt_max=%d===err_cnt_min=%d===cur tsk_fsm_cnt=%d tsk_fsm_cnt_min=%d tsk_fsm_cnt_max=%d tsk_fsm_cnt_avg=%2.2f \n",  delay, task_count, milliseconds, count_per_second, err_count_max,err_count_min, one_task_fsm_change_count, one_task_fsm_change_count_min, one_task_fsm_change_count_max, one_task_fsm_change_count_avg);
			//sleep(5); //观察用
		}
		one_task_fsm_change_count = 0;
		task_count++;
		break;

		case SYNC_LINK_STOP:
		ERROR_PRRINT("SYNC_LINK_STOP ========8======== \n");
		sleep(1);
		break;

		default:
		break;
		}

		if (err_count > ERROR_MAX_CNT) {
			get_link_info(&link_status);
			data_trans.snapshot.link_status = link_status;
			data_trans.snapshot.last_fsm = link_fsm;
			data_trans.snapshot.last_fsm_result = ret;
			data_trans.snapshot.task_count = task_count;
			clock_gettime(0, &data_trans.snapshot.time);
			ERROR_PRRINT("err_count:%d ================ \n", err_count);
			ERROR_PRRINT("snapshot.link_status:%d ================ \n", link_status);
			ERROR_PRRINT("snapshot.last_fsm:%d ================ \n", link_fsm);
			ERROR_PRRINT("snapshot.last_fsm_result:%d ================ \n", ret);
			ERROR_PRRINT("snapshot.task_count:%lld ================ \n", task_count);
			link_fsm = SYNC_LINK_STOP;
			err_count = 0;
		}

		usleep(LINK_FSM_USLEEP);
	
		if (err_count > err_count_max) {
			err_count_max = err_count;
		}
		if (err_count < err_count_min) {
			err_count_min = err_count;
		}
	}

}

int data_trans_init(unsigned int type)
{
	int ret;

	if (type > DEVICE_TYPE_MAX) {
		return -1;
	}

	srand((unsigned)time(NULL)); //保证随机数的随机性

	data_trans.ops  = data_trans_dev_ops[type];

	ret = data_trans.ops->init(type);
	if(ret < 0)
	{
		perror("init failed.\n");
		return -1;
	}

	if(pthread_create(&data_trans.tid , NULL , sync_fsm_translation, 0) == -1)
	{
		perror("pthread create error.\n");
		return -1;
	}

	return 0;
}
