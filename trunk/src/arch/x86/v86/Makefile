
V86MOD := dos

FILES := vm86.c vm86_int.c vm86_fault.c vm86_mem.c vm86_init.S
SRC += $(patsubst %, v86/%, $(filter %.c, $(FILES)))
SRC += $(patsubst %, v86/%, $(filter %.S, $(FILES)))

-include $(patsubst %, v86/%/Makefile, $(V86MOD))