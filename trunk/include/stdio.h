#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <kernel/types.h>
#include <string.h>
#include <drivers/video.h>

typedef int (*fnptr_t)(unsigned c, void **helper);

int do_printf(const char *fmt, va_list args, fnptr_t fn, void *ptr);
int kprintf_help(unsigned c, void **ptr);
void printf(const char *fmt, ...);
int vsprintf(char *buffer, const char *fmt, va_list arg_ptr);
int sprintf(char *buffer, const char *fmt, ...);
void panic(const char *fmt, ...);

#endif
