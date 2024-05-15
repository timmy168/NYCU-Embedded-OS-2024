obj-m += led_driver.o
# KDIR = /lib/modules/6.1.0-rpi7-rpi-v8/build
KDIR = /lib/modules/6.1.77-v8+/build
# KDIR = /lib/modules/$(shell uname -r)/build

all:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C $(KDIR) M=$(shell pwd) modules
#	make -C $(KDIR) M=$(shell pwd) modules
clean:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C $(KDIR) M=$(shell pwd) clean
#       make -C $(KDIR) M=$(shell pwd) clean
