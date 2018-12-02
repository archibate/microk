D=../
NAME=libl4
KTE=a

default: all

include $Ddirs.mak
include $Mbeg.mak


PHONY+=all
all: $(IMAGE)


PHONY+=install
install: $(IMAGE)
	cp $< $Nlib/

include $Minsinc.mak
include $Mend.mak
