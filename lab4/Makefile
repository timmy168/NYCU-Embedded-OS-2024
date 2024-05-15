obj-m += mydev.o
# KDIR = /lib/modules/$(shell uname -r)/build
KDIR = /lib/modules/6.1.77-v8+/build

all:
#	make -C $(KDIR) M=$(shell pwd) modules
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C $(KDIR) M=$(shell pwd) modules
	/usr/bin/aarch64-linux-gnu-gcc -o writer writer.c
	/usr/bin/aarch64-linux-gnu-g++ -o reader reader.c
clean:
#	make -C $(KDIR) M=$(shell pwd) modules
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C $(KDIR) M=$(shell pwd) clean
	rm writer
	rm reader