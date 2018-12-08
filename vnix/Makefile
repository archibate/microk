D=../
NAME=vnix
INCNAME=sys
KTE=bin
LDSCRIPT=vnix.ld

default: all

include $Ddirs.mak
include $Mbeg.mak


PHONY+=all
all: $Blibvnix.a

$Blibvnix.o: $Blibvnix.asm $(IMAGE)

$Blibvnix.a: $Blibvnix.o
	$(AR) cqs $@ $<


PHONY+=install
install: $Blibvnix.a $Bvnix.bin
	cp $Blibvnix.a $Nlib/
	cp $Bvnix.bin $Nsys/


include $Minsinc.mak
include $Mend.mak
