#ifndef _ELFHASH_H
#define _ELFHASH_H

typedef struct {
	Elf_Word nbucket;
	Elf_Word nchain;
	Elf_Word *bucket;
	Elf_Word *chain;
} Elf32_Hash;

static inline unsigned long
elf_hash(const unsigned char *name)
{
          unsigned long h = 0, g;
          while (*name)
          {
                  h = (h << 4) + *name++;
		  g = h & 0xf0000000;
                  if (g)
                         h ^= g >> 24;
                  h &= ~g;
          }
          return h;
}

static inline unsigned long
elf_gnu_hash (const char *s)
{
        unsigned long h = 5381;
	unsigned char c;

        for (c = *s; c != '\0'; c = *++s)
                h = h * 33 + c;

        return h;
}

#endif

