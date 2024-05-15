obj-m += hw1_driver.o
# KDIR = /lib/modules/$(shell uname -r)/build
KDIR = /lib/modules/6.1.77-v8+/build

all:
#	make -C $(KDIR) M=$(shell pwd) modules
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C $(KDIR) M=$(shell pwd) modules
	/usr/bin/aarch64-linux-gnu-gcc -o hw1 hw1.c
clean:
#   make -C $(KDIR) M=$(shell pwd) clean
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C $(KDIR) M=$(shell pwd) clean
	rm hw1
