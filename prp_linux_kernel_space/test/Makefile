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

TOPDIR  := $(shell pwd)

CFLAGS += $(DEBFLAGS)
CFLAGS += -I. -I..
CFLAGS += -Wall

TARGET = prp_test
OBJS   = $(TARGET)
SRC    = $(wildcard *.c)

all:	.depend $(TARGET)
	

$(TARGET): $(SRC:.c=.o)
	$(CC) -o $(TARGET) $^ -lpthread -lm

.c.o:
	$(CC) -c -o $*.o $*.c $(CFLAGS)

clean:

	@rm -f *.o .depend $(TARGET)

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > $@

ifeq (.depend,$(wildcard .depend))
include .depend
endif
