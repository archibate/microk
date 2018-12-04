D=../
NAME=init
KTE=bin

default: all

include $Ddirs.mak
include $Mbeg.mak


PHONY+=all
all: $(IMAGE)


PHONY+=install
install: $(IMAGE)


INCLUDE+=kbdsvr comsvr xterm pathsvr
include $Mend.mak
