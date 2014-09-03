KDIR  := /usr/src/linux
PWD   := $(shell pwd);

obj-m := mfw_module.o

mfw_module-objs := mfw.o procfs.o

all:
	$(MAKE) -Wall -C $(KDIR) SUBDIRS=$(PWD)

clean:
	$(RM) .*.cmd *.mod.c *.o *.ko -r .tmp* Module.symvers
