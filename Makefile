KDIR := $(KDIR)
MDIR := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
ODIR := $(MDIR)/out/$(VER)

$(info -- KDIR: $(KDIR))
$(info -- MDIR: $(MDIR))
$(info -- ODIR: $(ODIR))

.PHONY: all compdb clean

all:
	make -C $(KDIR) M=$(ODIR) src=$(MDIR) modules compile_commands.json
compdb:
	python3 $(MDIR)/.vscode/generate_compdb.py -O $(KDIR) $(ODIR)
clean:
	make -C $(KDIR) M=$(ODIR) src=$(MDIR) clean
