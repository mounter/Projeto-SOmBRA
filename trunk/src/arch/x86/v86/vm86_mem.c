#include <x86/v86.h>
#include <string.h>

void *v86_mem_read(void *dst, void *src, size_t sz)
{
    if ((unsigned)src >= 0x100000)
        return NULL;

    if (((unsigned)src+sz) >= 0x100000)
        sz = 0x100000 - (unsigned)src;

    if (sz > 0)
        return memcpy(dst, src, sz);

    return NULL;
}

void v86_video_write(void *ptr, unsigned ofs, size_t sz)
{
    unsigned video_addr = 0xA0000 + ofs;

    if (video_addr < 0xA0000)
        return;

    if ((video_addr+sz) >= 0xB0000)
        sz = 0xB0000 - video_addr;

    if (sz > 0)
        memcpy((void*)video_addr, ptr, sz);
}
