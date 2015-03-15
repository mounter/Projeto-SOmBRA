
FILES := fchar.c memcmp.c memcpy.c memset.c memsetw.c strcmp.c strcpy.c strlen.c strncmp.c \
	strncpy.c strtoupper.c strcat.c itoa.c
OBJ += $(patsubst %.c, string/%.o, $(filter %.c, $(FILES)))
OBJ += $(patsubst %.S, string/%.o, $(filter %.S, $(FILES)))
