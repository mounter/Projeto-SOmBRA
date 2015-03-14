
FILES := disable.c enable.c inportb.c inportl.c inportw.c outportb.c outportl.c outportw.c
OBJ += $(patsubst %.c, system/%.o, $(filter %.c, $(FILES)))
OBJ += $(patsubst %.S, system/%.o, $(filter %.S, $(FILES)))
