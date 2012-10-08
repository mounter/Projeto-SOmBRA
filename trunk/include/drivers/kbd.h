#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <kernel.h>
#include <kernel/types.h>

void keyboard_init();
int keyboard_irq();
void keyboard_bh();

#endif
