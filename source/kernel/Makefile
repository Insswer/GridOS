include $(MY_BUILD_BASE)/config.mk
include kernel.mk

ADD_INCLUDE +=$(HAL_INCLUDE) -include hal_config.h

#要编译的文件列表
OBJS	= cpu.o lock.o cs.o complete.o atomic.o mutex.o event.o ddk_for_drv.o srv.o handle.o
PARTS	= arch memory process

all:  subs_parts $(OBJS)
	$(LD) -r -o kernel.a *.o $(ARCH_LD_FLAGS)
	cd ../drivers/hal && make link
	
include $(MY_BUILD_BASE)/cc.mk
