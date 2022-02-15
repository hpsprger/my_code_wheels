#include <stdlib.h>
#include <stdio.h>
#include <time.h>

unsigned int attack_address = 0; //存放你没有访问权限的内存的地址
unsigned int index = 0;

unsigned char data0[256][4096] = {0};//测试访问速度的数据集合

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
    unsigned long long t0 = 0;
    unsigned long long t1 = 0;
    unsigned long long min_time = 0xffffffff;
    unsigned int min_time_index = 0;
    
    srand((int)time(NULL)); 
    index = rand() % 255;
        
    t0 = get_cycle_count();
    for (i = 0; i < 4096; i++) {
        sum += data0[index][i];
    }
    t1 = get_cycle_count();
    printf("index=%d delt_t=%lld \r\n", index, t1 - t0);
    
    for (i = 0; i < 256; i++) {
        t0 = get_cycle_count();
        for (j = 0; j < 4096; j++) {
            sum += data0[index][j];
        }
        t1 = get_cycle_count();
        printf("index=%d delt_t=%lld \n", i, t1 - t0);
        if ((t1 - t0) < min_time) {
			min_time = t1 - t0;
			min_time_index = i;
		}
    }
    
	printf("min_time_index=%d min_time=%lld \n", min_time_index, min_time);
	
    return 0;
}