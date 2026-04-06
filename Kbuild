obj := $(src)/out

MODULE_NAME := mylkm
$(MODULE_NAME)-objs := core.o
obj-m := $(MODULE_NAME).o
$(info -- KDIR: $(KDIR))
$(info -- MDIR: $(M))
$(info -- src: $(src))
$(info -- obj: $(obj))

lkmloader-objs := loader.o
obj-m += lkmloader.o

ccflags-y += -Wno-declaration-after-statement
ccflags-y += -Wno-unused-variable
ccflags-y += -Wno-int-conversion
ccflags-y += -Wno-unused-result
ccflags-y += -Wno-unused-function
ccflags-y += -Wno-builtin-macro-redefined
ccflags-y += -Wno-strict-prototypes
ccflags-y += -I$(srctree)
