ifeq ($(KERNELRELEASE),)

#家里的代码目录:cd /home/hpsp/rock_space/my_code_wheels/my_code_wheels/fast_ko/kernel_code
#find /usr/src/ -name "*generated*" ==> /usr/src/linux-headers-5.11.0-40-generic
#在家里x86-64电脑上运行时 编译内核模块用的源码树路径: KDIR ?= /usr/src/linux-headers-5.11.0-46-generic
#qemu上运行时 编译内核模块用的源码树路径: KDIR ?= /home/hpsp/rock_space/iMX6Q_files/linux_for_qemu/linux_for_qume/linux-4.14.222/build_for_qemu
#qemu上运行时 编译工具链:aarch64-linux-gnu-

KDIR ?= /home/hpsp/rock_space/iMX6Q_files/linux_for_qemu/linux_for_qume/linux-4.14.222/build_for_qemu

CROSS_COMPILE=aarch64-linux-gnu-

PWD := $(shell pwd)

#for debug compliing 
#make V=1 ==> during complling, display detailed compiling info 

#for cross compiling 
#make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=arm-linux-

#for x86-64 家里台式机
#modules:
#	make -C $(KDIR) M=$(PWD) modules V=1
	
#for qemu
modules:
	make -C $(KDIR) M=$(PWD) modules V=1 ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE)

.PHONEY:clean  
clean :
	rm -rf .*.cmd *.mod *.mod.c *.mod.o *.o *.ko modules.order Module.symvers 


else

#for depend on extra module symbols which located in current dir
#KBUILD_EXTRA_SYMBOLS = ./Module.symvers

kernel_module_name := rocklee_kernel
obj-m := $(kernel_module_name).o

#for multi files in ko 
#$(kernel_module_name)-objs := rock_gpio_led.o  rock_gpio_led_sub.o 

#for second ko module
kernel_module_name := rocklee_kernel_platformdrv
obj-m += $(kernel_module_name).o

kernel_module_name := rocklee_kernel_rbtree
obj-m += $(kernel_module_name).o

endif
