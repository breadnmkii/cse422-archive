# TODO: Change this to the location of your kernel source code
KERNEL_SOURCE=/project/scratch01/compile/b.z.yang/linux_source/linux

EXTRA_CFLAGS += -DMODULE=1 -D__KERNEL__=1

obj-m := vfs_thread.o

PHONY: all

all:
	$(MAKE) -C $(KERNEL_SOURCE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SOURCE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- M=$(PWD) clean 
