default: all

include $Ddirs.mak
include $Mbeg.mak


FDIMG=$Ba.img
BOOTSECT=$Dboot/$Bbootsect.bin

all: $(FDIMG)


APPS=$(shell cat apps.txt)


PHONY+=app-%
app-%:
	make -C $* install

PHONY+=app-%
app-inc-%:
	-make -C $* install-inc

$(BOOTSECT): app-boot


$(FDIMG): $(BOOTSECT) $(APPS:%=app-inc-%) $(APPS:%=app-%)
	mkdir -p $(@D)
	dd if=/dev/zero of=$@ bs=$$((1440*1024)) count=1
	mkfs.fat --invariant -S 512 -s 8 -F 12 -R 8 -n "OSYS FLOPPY" $@
	dd if=$(BOOTSECT) of=$@ bs=1 skip=62 seek=62 count=$$((512*4-62)) conv=notrunc
	mkdir -p $Bmnt
	-$(SH) $Sumountloop.sh $Bmnt
	$(SH) $Smountloop.sh $Bmnt $@
	sudo cp -r isodir/kernel.bin isodir/bin $Bmnt
	$(SH) $Sumountloop.sh $Bmnt


PHONY+=bochsrun
bochsrun: $(FDIMG)
	$(SH) $Sbochs.sh

PHONY+=run
run: $(FDIMG)
	$(SH) $Sqemu.sh

PHONY+=test
test: $(FDIMG)
	-rm -rf /tmp/l4benchsamps
	$(SH) $Sqemu.sh -t

PHONY+=bench
bench:
	make test
	$(PYTHON) $Sbenchplot.py /tmp/l4benchsamps
	display figure.png

PHONY+=debug
debug: $(FDIMG)
	@#@echo "*** now run 'gdb' in another terminal." >&2
	$(SH) $Sqemu.sh -d



include $Mrules.mak
include $Mendup.mak
