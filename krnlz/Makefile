D=../
NAME=krnlz
KTE=bin
LDSCRIPT=krnlz.ld

default: all

include $Ddirs.mak
include $Mbeg.mak


PHONY+=all
all: $(IMAGE)


ucmphdr.asm: $Bkernel.bin.z

$Bkernel.bin.z: $Nkernel.bin
	$(COMPRESS) $< $@

PHONY+=install
install: $(IMAGE)
	cp $< $Nkrnlz.bin

include $Mend.mak
