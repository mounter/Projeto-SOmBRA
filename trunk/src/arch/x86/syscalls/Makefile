
FILES := getprocess.c ubrk.c syscalls.h
SRC += $(patsubst %, syscalls/%, $(filter %.c, $(FILES)))
SRC += $(patsubst %, syscalls/%, $(filter %.S, $(FILES)))
