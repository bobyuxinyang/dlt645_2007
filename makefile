#
# FreeModbus Linux PORT - Makefile
#
# Copyright (c) 2006 Christian Walter, ? s::can, Vienna 2006.
#
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# platform dependencies
# ---------------------------------------------------------------------------
ifneq ($(strip $(shell gcc -v 2>&1 |grep "cygwin")),)
	CYGWIN_BUILD = YES
endif
CC	    = gcc
CXX	    = g++
OBJCOPY	    = objcopy
INSIGHT     = /opt/insight-x86/bin/insight

# ---------------------------------------------------------------------------
# project specifics
# ---------------------------------------------------------------------------
CFLAGS	    = -MD -g3 -Wall
LDFLAGS     =
ifeq ($(CYGWIN_BUILD),YES)
else
#LDFLAGS     += -lpthread
endif

TGT         = 1
OTHER_CSRC  = 
OTHER_ASRC  = 
CSRC        = dlt645_api_07.c main.c
ASRC        = 
OBJS        = $(CSRC:.c=.o) $(ASRC:.S=.o)
NOLINK_OBJS = $(OTHER_CSRC:.c=.o) $(OTHER_ASRC:.S=.o)
DEPS        = $(OBJS:.o=.d) $(NOLINK_OBJS:.o=.d)
BIN         = $(TGT)

.PHONY: clean all

all: $(BIN)

debug:
	$(INSIGHT) --se=$(TGT)

$(BIN): $(OBJS) $(NOLINK_OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

clean:
	rm -f $(DEPS)
	rm -f $(OBJS) $(NOLINK_OBJS)
	rm -f $(BIN)

# ---------------------------------------------------------------------------
# rules for code generation
# ---------------------------------------------------------------------------
%.o:    %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o:    %.S
	$(CC) $(ASFLAGS) -o $@ -c $<

# ---------------------------------------------------------------------------
#  # compiler generated dependencies
# ---------------------------------------------------------------------------
-include $(LWOS_DEPS) $(PORT_DEPS) $(APPL_DEPS)

