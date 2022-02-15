#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define DATA_ARRAY_SIZE_0  256
#define DATA_ARRAY_SIZE_1  4096

unsigned int attack_address = 0; //存放你没有访问权限的内存的地址
unsigned int index = 0;

unsigned char data0[DATA_ARRAY_SIZE_0][DATA_ARRAY_SIZE_1] = {0};//测试访问速度的数据集合

unsigned long long get_cycle_count()
{
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo; 
}

int main( void )
{
    unsigned int i,j;
    unsigned int sum = 0;
    unsigned long long t0_0 = 0;
    unsigned long long t1_0 = 0;
	unsigned long long t0 = 0;
    unsigned long long t1 = 0;
    unsigned long long min_time = 0xffffffff;
    unsigned int min_time_index = 0;
    
    srand((int)time(NULL)); 
    index = rand() % (DATA_ARRAY_SIZE_0 - 1);
        
    t0_0 = get_cycle_count();
    for (i = 0; i < DATA_ARRAY_SIZE_1; i++) {
        sum += data0[index][i];
    }
    t1_0 = get_cycle_count();
    
    for (i = 0; i < DATA_ARRAY_SIZE_0; i++) {
        t0 = get_cycle_count();
        for (j = 0; j < DATA_ARRAY_SIZE_1; j++) {
            sum += data0[i][j];
        }
        t1 = get_cycle_count();
        printf("i=%d delt_t=%lld \n", i, t1 - t0);
        if ((t1 - t0) < min_time) {
			min_time = t1 - t0;
			min_time_index = i;
		}
    }
    
	printf("index=%d delt_t=%lld \n", index, t1_0 - t0_0);
	printf("min_time_index=%d min_time=%lld \n", min_time_index, min_time);
	
    return 0;
}