
FILES := video.c kbd.c timer.c rtc.c ps2.c mouse.c
SRC += $(patsubst %, drivers/%, $(filter %.c, $(FILES)))
SRC += $(patsubst %, drivers/%, $(filter %.S, $(FILES)))
