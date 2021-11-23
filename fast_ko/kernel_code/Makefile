ifeq ($(KERNELRELEASE),)

#find /usr/src/ -name "*generated*" ==> /usr/src/linux-headers-5.11.0-40-generic
KDIR ?= /usr/src/linux-headers-5.11.0-40-generic

PWD := $(shell pwd)

modules:
	make -C $(KDIR) M=$(PWD)   modules


.PHONEY:clean  
clean :
	rm -rf .*.ko.cmd *.ko *.mod.c *.mod.o .*.mod.o.cmd *.o .*.o.cmd *.order *.symvers .tmp_versions  *.o.cmd *.mod *.cmd  .modules.order.cmd .Module.symvers.cmd  .kernel_ko.mod.cmd


else

#for depend on extra module symbols which located in current dir
#KBUILD_EXTRA_SYMBOLS = ./Module.symvers

kernel_module_name := kernel_ko
obj-m := $(kernel_module_name).o

#for multi files in ko 
#$(kernel_module_name)-objs := rock_gpio_led.o  rock_gpio_led_sub.o 

#for second ko module
#kernel_module_name := rock_net_card
#obj-m += $(kernel_module_name).o

endif
