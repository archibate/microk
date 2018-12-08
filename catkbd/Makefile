D=../
NAME=catkbd
KTE=elf

default: all

include $Ddirs.mak
include $Mbeg.mak


PHONY+=all
all: $(IMAGE)


PHONY+=install
install: $(IMAGE)
	cp $< $Nbin/$(NAME)


include $Mend.mak
