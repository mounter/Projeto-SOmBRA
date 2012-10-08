
FILES := kphelp.c panic.c printf.c ksphelp.c vsprintf.c vsnprintf.c sprintf.c
OBJ += $(patsubst %.c, stdio/%.o, $(filter %.c, $(FILES)))
OBJ += $(patsubst %.S, stdio/%.o, $(filter %.S, $(FILES)))
