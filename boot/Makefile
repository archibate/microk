D=../
NAME=bootsect
KTE=bin

default: all

include $Ddirs.mak
include $Mbeg.mak


PHONY+=all
all: $(IMAGE)


PHONY+=install
install: $(IMAGE)
	cp $< $Nkernel.bin


include $Mrules.mak
include $Mendup.mak
