# *********************************************************************
# *   _____       ______  _____                                       *
# *  |_   _|     |  ____|/ ____|                                      *
# *    | |  _ __ | |__  | (___    Institute of Embedded Systems       *
# *    | | | '_ \|  __|  \___ \   Zurich University of Applied        *
# *   _| |_| | | | |____ ____) |  Sciences                            *
# *  |_____|_| |_|______|_____/   8401 Winterthur, Switzerland        *
# *                                                                   *
# *********************************************************************
# * 
# *  Project     : Parallel Redundancy Protocol
# * 
# *  Version     : 1.0
# *  Author      : Sven Meier
# * 
# *********************************************************************
# *  Change History
# *
# *  Date     | Name     | Modification
# ************|**********|*********************************************
# *  17.12.07 | mesv     | file created
# *  30.11.15 | beti     | copied from prp-0
# *********************************************************************

ifneq ($(KERNELRELEASE),)
# 1st pass of kernel module builder
obj-m = prp_dev.o
prp_dev-objs += PRP_NetItf_T_linux.o
prp_dev-objs += PRP_OsItf_T_linux.o
prp_dev-objs += ../prp/PRP_DiscardAlgorithm_T.o
prp_dev-objs += ../prp/PRP_Environment_T.o
prp_dev-objs += ../prp/PRP_EnvironmentConfiguration_T.o
prp_dev-objs += ../prp/PRP_Frames_T.o
prp_dev-objs += ../prp/PRP_FrameAnalyser_T.o
prp_dev-objs += ../prp/PRP_LogItf_T.o
prp_dev-objs += ../prp/PRP_RedundancyControlTrailer_T.o
prp_dev-objs += ../prp/PRP_Supervision_T.o
prp_dev-objs += ../prp/PRP_T.o
prp_dev-objs += ../prp/PRP_Timer_T.o
prp_dev-objs += ../prp/PRP_Trailer_T.o
prp_dev-objs += ../prp/PRP_Lock_T.o


# uncomment this for debug log
EXTRA_CFLAGS += -DPRP_DEBUG_LOG

# uncomment this to have source file names / line numbers
# in the debug log, handy for driver module debugging
#EXTRA_CFLAGS += -DPRP_DEBUG_LOG_SRC

else
# 2nd pass of kernel module builder
KDIR	:= /lib/modules/$(shell uname -r)/build
# use the follwing line to hard code the kernel path
#KDIR	:= /lib/modules/<kernel-version>/build
KDIR	:= /lib/modules/4.2.0-18-generic/build
PWD	:= $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm ../prp/*.o
	rm ../prp/.*.o.cmd
	
endif

