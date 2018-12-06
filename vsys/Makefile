D=../
NAME=vsys
INCNAME=.
KTE=bin
LDSCRIPT=vsys.ld

default: all

include $Ddirs.mak
include $Mbeg.mak


PHONY+=all
all: $Blibvsys.a

$Blibvsys.o: $Blibvsys.asm $(IMAGE)

$Blibvsys.a: $Blibvsys.o
	$(AR) cqs $@ $<


PHONY+=install
install: $Blibvsys.a $Bvsys.bin
	cp $Blibvsys.a $Nlib/
	cp $Bvsys.bin $Nsys/


include $Minsinc.mak
include $Mend.mak
