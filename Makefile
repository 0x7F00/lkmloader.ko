KDIR := $(KDIR)
MDIR := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

$(info -- KDIR: $(KDIR))
$(info -- MDIR: $(MDIR))

.PHONY: all compdb clean

all:
	make -C $(KDIR) M=$(MDIR)/out src=$(MDIR) modules
compdb:
	python3 $(MDIR)/.vscode/generate_compdb.py -O $(KDIR) $(MDIR)/out
clean:
	make -C $(KDIR) M=$(MDIR)/out src=$(MDIR) clean
