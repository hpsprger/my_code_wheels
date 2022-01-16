ifeq ($(KERNELRELEASE),)

#find /usr/src/ -name "*generated*" ==> /usr/src/linux-headers-5.11.0-40-generic
KDIR ?= /usr/src/linux-headers-5.11.0-46-generic

PWD := $(shell pwd)

#for debug compliing 
#make V=1 ==> during complling, display detailed compiling info 

#for cross compiling 
#make -C $(KDIR) M=$(PWD) modules CROSS_COMPILE=arm-linux-

modules:
	make -C $(KDIR) M=$(PWD) modules V=1


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
