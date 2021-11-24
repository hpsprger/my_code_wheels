#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>

void main(int argc, char* argv[])
{
    char *device_node= "/dev/rocklee_cdev0";
    int fd, ret;
    
    if(argc < 2)
    {
        printf("usage: ./ledtest 1 1\n");
        exit(1);
    }

    if((fd = open(device_node, O_RDWR|O_NOCTTY|O_NDELAY))<0) {
        printf("Open %s faild\n", device_node);
        exit(1);
    }

    printf("ioctl --> atoi(argv[0])=%d  atoi(argv[1])=%d  atoi(argv[2])=%d \n",atoi(argv[0]),atoi(argv[1]), atoi(argv[2]));

    ret = ioctl(fd, atoi(argv[1]), atoi(argv[2]));

    close(fd);
    return ;
}


