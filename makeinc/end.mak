PHONY+=always
always:
	@:


INCLUDE+=. uinc $Ninc
CFLAGS+=$(INCLUDE:%=-I%)

LIBPATH+=$Nlib
LDFLAGS+=$(LIBPATH:%=-L%)

ONAMES:=$(shell cat srcs.txt)
OBJS:=$(ONAMES:%=$B%.o)

LIBS+=$(shell cat libs.txt)

GCC_LIB+=$(shell $(CC) $(CFLAGS) -print-libgcc-file-name)
OBJS+=$(GCC_LIB)


include $Mrules.mak
include $Mmk-bin.mak
include $Mautodep.mak
include $Mendup.mak
