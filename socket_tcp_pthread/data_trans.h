#ifndef __DATA_TRANS_H__
#define __DATA_TRANS_H__

#include <time.h>
#include "phy_eth_common.h"

#define NORMAL_PRINTF  //printf
#define ERROR_PRRINT   printf

typedef struct _snapshot_info {
	unsigned int last_fsm;
	unsigned int last_fsm_result;
	unsigned int link_status;
	unsigned int task_count;
	struct timeval time;
} snapshot_info;

typedef struct _data_trans_device {
	pthread_t tid;
	device_ops *ops;
	snapshot_info snapshot;
} data_trans_device;

#endif