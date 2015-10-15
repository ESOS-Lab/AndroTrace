obj-m := ../androtrace_module.o
	     
KDIR := ../../
CCPATH := /opt/toolchains/aarch64-linux-android-4.9/bin/aarch64-linux-android- 
default:
#	$(MAKE) ARCH=arm64 CROSS_COMPILE=$(CCPATH) -C $(KDIR) M=$(PWD) modules
	$(MAKE) ARCH=arm64 CROSS_COMPILE=$(CCPATH) -C $(KDIR) M=$(PWD) modules CFLAGS_MODULE=-fno-pic

clean:
	make -C $(KDIR) M=$(PWD) clean

