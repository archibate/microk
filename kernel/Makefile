D=../
INCNAME=krnl4
NAME=kernel
KTE=bin
LDSCRIPT=kernel.ld

default: all

include $Ddirs.mak
include $Mbeg.mak


PHONY+=all
all: $(IMAGE)


PHONY+=install
install: $(IMAGE)
	cp $< $Nkernel.bin

include $Minsinc.mak
include $Mend.mak
