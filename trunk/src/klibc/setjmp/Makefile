
FILES := longjmp.c setjmp.S
OBJ += $(patsubst %.c, setjmp/%.o, $(filter %.c, $(FILES)))
OBJ += $(patsubst %.S, setjmp/%.o, $(filter %.S, $(FILES)))
