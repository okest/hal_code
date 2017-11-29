#zjc

CROSS_COMPILE = /opt/poky/1.7.3/sysroots/x86_64-pokysdk-linux/usr/bin/arm-linux-gnueabihf-
CC = $(CROSS_COMPILE)gcc

KO_NAME = aux_det

ifeq ($(KERNELRELEASE),)

KERN_DIR = /home/foryou/代码管理/11-22/iran_pro_linux/rn4yd6_kernel

CUR_DIR = $(shell pwd)

all : 
	make  -C $(KERN_DIR) M=$(CUR_DIR) modules

clean : 
	rm *.ko *.o modules.order Module.symvers

install :
	cp -raf  *.ko   ~/share/


else

obj-m += $(KO_NAME).o

endif
