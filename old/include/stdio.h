#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <kernel/types.h>
#include <string.h>
#include <x86-common/drivers/video.h>

typedef int (*fnptr_t)(unsigned c, void **helper, size_t *counter, size_t maxcounter);

int do_printf(const char *fmt, va_list args, fnptr_t fn, void *ptr, size_t maxcounter);
int kprintf_help(unsigned c, void **ptr, size_t *counter, size_t maxcounter);

void printf(const char *fmt, ...);
int vsprintf(char *buffer, const char *fmt, va_list arg_ptr);
int vsnprintf(char *buffer, size_t count, const char *fmt, va_list arg_ptr);
int sprintf(char *buffer, const char *fmt, ...);
void panic(const char *fmt, ...);

#endif
