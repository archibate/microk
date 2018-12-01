PHONY+=always
always:
	@:


INCLUDE+=. $Dkernel/uinc $Dlibc $Dincl
CFLAGS+=$(INCLUDE:%=-I%)

ONAMES:=$(shell cat srcs.txt)
OBJS:=$(ONAMES:%=$B%.o)

LNAMES:=$(shell cat libs.txt)
OBJS+=$(LNAMES:%=$Llib%.a)

GCC_LIB+=$(shell $(CC) $(CFLAGS) -print-libgcc-file-name)
OBJS+=$(GCC_LIB)


include $Mrules.mak
include $Mmk-bin.mak
include $Mautodep.mak
include $Mendup.mak
