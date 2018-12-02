D=../
NAME=libc
KTE=a

default: all

include $Ddirs.mak
include $Mbeg.mak


PHONY+=all
all: $(IMAGE)


PHONY+=install
install: $(IMAGE)
	cp $< $Nlib/

PHONY+=install-inc
install-inc: $(shell cat incs.txt)
	cp -r $^ $Ninc


include $Mend.mak
