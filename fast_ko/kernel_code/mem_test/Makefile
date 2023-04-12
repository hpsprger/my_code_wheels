############# 简易的写法
## KVERS = $(shell uname -r)
## 
## # Kernel modules
## obj-m += proc.o
## 
## # Specify flags for the module compilation.
## #EXTRA_CFLAGS=-g -O0
## 
## build: kernel_modules
## 
## kernel_modules:
## 	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules
## 
## clean:
## 	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) clean
## 
## #using : ./a.out xxx(address) 4
## app:
## 	gcc meltdown-baohua.c -O2 -msse2
## 
## for debug compliing 
## make V=1 ==> during complling, display detailed compiling info 
## for cross compiling 
## make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=arm-linux-
## 
## for x86-64 家里台式机
## 	kernel_modules:
## 		make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules
#############

PWD := $(shell pwd)
KVERS = $(shell uname -r)

KDIR ?= /home/hpsp/rock_space/rockpi_4b/rockchip-bsp/kernel
CROSS_COMPILE=aarch64-linux-gnu-

# Specify flags for the module compilation.
#-O0 linux4.19 编译内核模块时 编译报错，5.xx内核没有问题
EXTRA_CFLAGS += -g -O1

# Kernel modules
kernel_module_name := mem_test
obj-m += $(kernel_module_name).o

build: kernel_modules

kernel_modules:
	make -C $(KDIR) M=$(PWD) modules V=1 ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE)

clean:
	rm -rf .*.cmd *.mod *.mod.c *.mod.o *.o *.o.d *.ko modules.order Module.symvers 
	
mount_img:
	cd  /home/hpsp/rock_space/iMX6Q_files/initrd_for_qemu   
	mkdir  -p /home/hpsp/rock_space/iMX6Q_files/initrd_for_qemu/mount 
	sudo mount  /home/hpsp/rock_space/iMX6Q_files/initrd_for_qemu/initrd_64le_250M.ext4  /home/hpsp/rock_space/iMX6Q_files/initrd_for_qemu/mount

install:
	sudo cp ./mem_test.ko /home/hpsp/rock_space/iMX6Q_files/initrd_for_qemu/mount	
	sudo chmod -Rf 777  /home/hpsp/rock_space/iMX6Q_files/initrd_for_qemu/mount	