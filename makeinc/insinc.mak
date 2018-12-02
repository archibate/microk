ifndef INCNAME
INCNAME=$(NAME)
endif

PHONY+=install-inc
install-inc: uinc
	mkdir -p $Ninc/$(INCNAME)
	cp -r $</* $Ninc/$(INCNAME)
