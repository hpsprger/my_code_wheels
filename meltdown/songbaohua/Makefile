KVERS = $(shell uname -r)

# Kernel modules
obj-m += proc.o

# Specify flags for the module compilation.
#EXTRA_CFLAGS=-g -O0

build: kernel_modules

kernel_modules:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules

clean:
	make -C /lib/modules/$(KVERS)/build M=$(CURDIR) clean

#using : ./a.out xxx(address) 4
app:
	gcc meltdown-baohua.c -O2 -msse2