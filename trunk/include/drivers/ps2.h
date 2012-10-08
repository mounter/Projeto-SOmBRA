#ifndef _PS2_H
#define _PS2_H

#include <kernel/types.h>

#define PS2_PORT	0x60
#define PS2_CTRL	0x64
#define PS2_ACK		0xFA

int ps2_wait(uint8_t w_type);
void ps2_write(uint32_t port, uint8_t data);
uint8_t ps2_writeack(uint32_t port, uint8_t data);
uint32_t ps2_read(void);

#endif
