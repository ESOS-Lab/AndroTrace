obj-m := ../androtrace_module.o
	     
KDIR := ../../
CCPATH := /opt/toolchains/arm-eabi-4.6-master/bin
default:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(CCPATH)/arm-eabi- -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean

