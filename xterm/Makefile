D=../
NAME=xterm
KTE=elf

default: all

include $Ddirs.mak
include $Mbeg.mak


PHONY+=all
all: $(IMAGE)


PHONY+=install
install: $(IMAGE)
	cp $< $Nsys/$(NAME)


include $Mend.mak
