PHONY+=always
always:
	@:


INCLUDE+=. $Dkernel/uinc $Dlibc $Dlibu
CFLAGS+=$(INCLUDE:%=-I%)

ONAMES:=$(shell cat objs.txt)
OBJS:=$(ONAMES:%=$B%.o)

ANAMES:=$(shell cat libs.txt)
OBJS+=$(foreach x, $(ANAMES), $D$x/bin/$x.a)

GCC_LIB+=$(shell $(CC) $(CFLAGS) -print-libgcc-file-name)
OBJS+=$(GCC_LIB)


include $Mrules.mak
include $Mmk-bin.mak
include $Mautodep.mak
include $Mendup.mak
