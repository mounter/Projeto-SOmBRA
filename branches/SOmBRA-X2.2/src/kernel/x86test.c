#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "../arch/x86_64/x86emu/include/x86emu.h"
#include <system.h>
#include <time.h>

// Macros para com numeração e dados da BIOS

#define STR_SIZE 128

#define VBIOS_ROM	0xc0000
#define VBIOS_ROM_SIZE	0x10000

#define SBIOS_ROM	0xf0000
#define SBIOS_ROM_SIZE	0x10000

#define VBIOS_MEM	0xa0000
#define VBIOS_MEM_SIZE	0x10000

#define VBE_BUF		0x8000

#define ADD_RES(w, h, f, i) \
  res[res_cnt].width = w, \
  res[res_cnt].height = h, \
  res[res_cnt].vfreq = f, \
  res[res_cnt++].il = i;

 // Estrutura para rodar a vm x86
typedef struct
{
  x86emu_t *emu;
  unsigned char *video_mem;
} vm_t;

// Opções para o VM
struct
{
  unsigned port;
  unsigned port_set:1;
  unsigned verbose;
  unsigned force:1;
  unsigned timeout;

  unsigned all_modes:1;
  unsigned mode;
  unsigned mode_set:1;

  unsigned trace_flags;
  unsigned dump_flags;

  unsigned bios_map_all:1;
  unsigned bios_int;
} opt;

// Funções do log
void flush_log(x86emu_t *emu, char *buf, unsigned size);

// Funções de leitura e escrita...
unsigned vm_read_segofs16(x86emu_t *emu, unsigned addr);
void vm_write_byte(x86emu_t *emu, unsigned addr, unsigned val, unsigned perm);
void vm_write_word(x86emu_t *emu, unsigned addr, unsigned val, unsigned perm);
void vm_write_dword(x86emu_t *emu, unsigned addr, unsigned val, unsigned perm);
void copy_to_vm(x86emu_t *emu, unsigned dst, unsigned char *src, unsigned size, unsigned perm);
void copy_from_vm(x86emu_t *emu, void *dst, unsigned src, unsigned len);

// Funções para interrupções, inicialização e liberação do VM
int do_int(x86emu_t *emu, u8 num, unsigned type);
vm_t *vm_new(void);
void vm_free(vm_t *vm);
unsigned vm_run(x86emu_t *emu, time_t *t);
int vm_prepare(vm_t *vm);

// Informações da VBE
void print_vbe_info(vm_t *vm, x86emu_t *emu, unsigned mode);
void list_modes(vm_t *vm, unsigned mode);

// Testando a VBE
void probe_all(vm_t *vm);
int probe_port(vm_t *vm, unsigned port);

/*
* Implementação das funções
*/

// Funções do log
void flush_log(x86emu_t *emu, char *buf, unsigned size)
{
	if(!buf || !size) return;

	printf("Log: ");
	int i = 0;
	for (;i < size; i++)
		printf("%c", buf[i]);
	printf("\n");
}

// Funções de leitura e escrita...
unsigned vm_read_segofs16(x86emu_t *emu, unsigned addr)
{
	return x86emu_read_word(emu, addr) + (x86emu_read_word(emu, addr + 2) << 4);
}

void vm_write_byte(x86emu_t *emu, unsigned addr, unsigned val, unsigned perm)
{
	x86emu_write_byte_noperm(emu, addr, val);
	x86emu_set_perm(emu, addr, addr, perm | X86EMU_PERM_VALID);
}

void vm_write_word(x86emu_t *emu, unsigned addr, unsigned val, unsigned perm)
{
	x86emu_write_byte_noperm(emu, addr, val);
	x86emu_write_byte_noperm(emu, addr + 1, val >> 8);
	x86emu_set_perm(emu, addr, addr + 1, perm | X86EMU_PERM_VALID);
}

void vm_write_dword(x86emu_t *emu, unsigned addr, unsigned val, unsigned perm)
{
	x86emu_write_byte_noperm(emu, addr, val);
	x86emu_write_byte_noperm(emu, addr + 1, val >> 8);
	x86emu_write_byte_noperm(emu, addr + 2, val >> 16);
	x86emu_write_byte_noperm(emu, addr + 3, val >> 24);
	x86emu_set_perm(emu, addr, addr + 3, perm | X86EMU_PERM_VALID);
}

void copy_to_vm(x86emu_t *emu, unsigned dst, unsigned char *src, unsigned size, unsigned perm)
{
	if(!size) return;

	while(size--)
		vm_write_byte(emu, dst++, *src++, perm);
}

void copy_from_vm(x86emu_t *emu, void *dst, unsigned src, unsigned len)
{
	unsigned char *p = dst;
	unsigned u;

	for(u = 0; u < len; u++)
	{
		p[u] = x86emu_read_byte_noperm(emu, src + u);
	}
}

// Funções para interrupções, inicialização e liberação do VM

int do_int(x86emu_t *emu, u8 num, unsigned type)
{
	if((type & 0xff) == INTR_TYPE_FAULT)
	{
		x86emu_stop(emu);

		return 0;
	}

	// ignore ints != (0x10 or 0x42 or 0x6d)
	//if(num != 0x10 && num != 0x42 && num != 0x6d) return 1;

	return 0;
}

vm_t *vm_new()
{
	vm_t *vm;

	vm = calloc(1, sizeof *vm);

	vm->emu = x86emu_new(0, X86EMU_PERM_RW);
	/*vm->emu->private = vm;

	x86emu_set_log(vm->emu, 200000000, flush_log);
	x86emu_set_intr_handler(vm->emu, do_int);*/

	return vm;
}

void vm_free(vm_t *vm)
{
	x86emu_done(vm->emu);
	free(vm);
}

unsigned vm_run(x86emu_t *emu, time_t *t)
{
	unsigned err;

	if(opt.verbose >= 2)
		x86emu_log(emu, "=== emulation log ===\n");

	*t = time(0);

	x86emu_reset_access_stats(emu);

	err = x86emu_run(emu, X86EMU_RUN_LOOP | X86EMU_RUN_NO_CODE | X86EMU_RUN_TIMEOUT);

	*t = time(0) - *t;

	if(opt.dump_flags)
	{
		x86emu_log(emu, "\n; - - - final state\n");
		x86emu_dump(emu, opt.dump_flags);
		x86emu_log(emu, "; - - -\n");
	}

	if(opt.verbose >= 2)
		x86emu_log(emu, "=== emulation log end ===\n");

	x86emu_clear_log(emu, 1);

	return err;
}

int vm_prepare(vm_t *vm)
{
	int ok = 0;
	unsigned u;

	if(opt.verbose >= 2)
		printf("=== bios setup ===\n");

	// BIOS START COPY
	
	// Copy the first 4096 bytes
	copy_to_vm(vm->emu, 0x10*4, (unsigned char*)(0x10*4), 4, X86EMU_PERM_RW);
	copy_to_vm(vm->emu, 0x42*4, (unsigned char*)(0x42*4), 4, X86EMU_PERM_RW);
	copy_to_vm(vm->emu, 0x6d*4, (unsigned char*)(0x6d*4), 4, X86EMU_PERM_RW);	// original int 0x10
	copy_to_vm(vm->emu, 0x400,  (unsigned char*)(0x400), 0x100, X86EMU_PERM_RW);

	// VBIOS ROM Copy
	copy_to_vm(vm->emu, VBIOS_ROM, (unsigned char*)(VBIOS_ROM), opt.bios_map_all ? VBIOS_ROM_SIZE : (VBIOS_ROM+2) * 0x200, X86EMU_PERM_RX);

	if(opt.bios_map_all)
		copy_to_vm(vm->emu, SBIOS_ROM, (unsigned char*)(SBIOS_ROM), SBIOS_ROM_SIZE, X86EMU_PERM_RX);

	// BIOS END COPY

	if(opt.verbose >= 2)
	{
		printf("video bios: seg 0x%04x, size 0x%04x\n",
			VBIOS_ROM >> 4, x86emu_read_byte(vm->emu, VBIOS_ROM + 2) * 0x200
		);
		
		printf("video bios (0x10): entry 0x%04x:0x%04x\n",
			x86emu_read_word(vm->emu, 0x10*4 +  2),
			x86emu_read_word(vm->emu, 0x10*4)
		);
		
		printf("video bios (0x42): entry 0x%04x:0x%04x\n",
			x86emu_read_word(vm->emu, 0x42*4 +  2),
			x86emu_read_word(vm->emu, 0x42*4)
		);
		
		printf("video bios (0x6d): entry 0x%04x:0x%04x\n",
			x86emu_read_word(vm->emu, 0x6d*4 +  2),
			x86emu_read_word(vm->emu, 0x6d*4)
		);
		
		if(opt.bios_map_all) {
			printf("system bios: seg 0x%04x, size 0x%04x\n", SBIOS_ROM >> 4, SBIOS_ROM_SIZE);
		}
	}

	// video memory
	vm->video_mem = (unsigned char*)VBIOS_MEM;

	if(vm->video_mem)
	{
		x86emu_set_perm(vm->emu, VBIOS_MEM, VBIOS_MEM + VBIOS_MEM_SIZE - 1, X86EMU_PERM_RW);
		
		for(u = 0; u < VBIOS_MEM_SIZE; u += X86EMU_PAGE_SIZE)
		{
			x86emu_set_page(vm->emu, VBIOS_MEM + u, vm->video_mem + u);
		}
	}

	// start address 0:0x7c00
	x86emu_set_seg_register(vm->emu, vm->emu->x86.R_CS_SEL, 0);
	vm->emu->x86.R_EIP = 0x7c00;

#define ENTRY_OK(a) x86emu_read_byte_noperm(vm->emu, (x86emu_read_word(vm->emu, (a)*4 + 2) << 4) + x86emu_read_word(vm->emu, (a)*4))

	switch(opt.bios_int)
	{
		case 1:
			u = 0x10;
		break;

		case 2:
			u = 0x42;
		break;

		case 3:
			u = 0x6d;
		break;

		default:
			u = 0x10;

			if(ENTRY_OK(0x10))
			{
				u = 0x10;
			}
			else if(ENTRY_OK(0x6d))
			{
				u = 0x6d;
			}
			else if(ENTRY_OK(0x42))
			{
				u = 0x42;
			}
	}

	printf("video bios: using int 0x%02x\n", u);

	// int 0x10 ; hlt
	vm_write_byte(vm->emu, 0x7c00, 0xcd, X86EMU_PERM_RX);
	vm_write_byte(vm->emu, 0x7c01,    u, X86EMU_PERM_RX);
	vm_write_byte(vm->emu, 0x7c02, 0xf4, X86EMU_PERM_RX);

	// stack & buffer space
	x86emu_set_perm(vm->emu, VBE_BUF, 0xffff, X86EMU_PERM_RW);

	vm->emu->log.trace = opt.trace_flags;

	if(opt.timeout) vm->emu->timeout = opt.timeout ?: 20;

	ok = 1;

	return ok;
}

// Informações da VBE

void print_vbe_info(vm_t *vm, x86emu_t *emu, unsigned mode)
{
	unsigned char buf2[0x100];
	unsigned u, ml;
	unsigned modelist[0x100];
	unsigned modes, number;
	int err;
	time_t d;
	char s[64];
	unsigned version, oem_version, memory, attributes, width, height, bytes_p_line;
	unsigned win_A_start, win_B_start, win_A_attr, win_B_attr, win_gran, win_size;
	unsigned bpp, res_bpp, fb_start, pixel_clock;

	version = x86emu_read_word(emu, VBE_BUF + 0x04);
	oem_version = x86emu_read_word(emu, VBE_BUF + 0x14);
	memory = x86emu_read_word(emu, VBE_BUF + 0x12) << 16;

	if(!mode)
	{
		printf(
			"version = %u.%u, oem version = %u.%u\n",
			version >> 8, version & 0xff, oem_version >> 8, oem_version & 0xff
		);

		printf("memory = %uk\n", memory >> 10);

		buf2[sizeof buf2 - 1] = 0;

		u = vm_read_segofs16(emu, VBE_BUF + 0x06);
		copy_from_vm(emu, buf2, u, sizeof buf2 - 1);
		printf("oem name [0x%05x] = \"%s\"\n", u, buf2);

		u = vm_read_segofs16(emu, VBE_BUF + 0x16);
		copy_from_vm(emu, buf2, u, sizeof buf2 - 1);
		printf("vendor name [0x%05x] = \"%s\"\n", u, buf2);

		u = vm_read_segofs16(emu, VBE_BUF + 0x1a);
		copy_from_vm(emu, buf2, u, sizeof buf2 - 1);
		printf("product name [0x%05x] = \"%s\"\n", u, buf2);

		u = vm_read_segofs16(emu, VBE_BUF + 0x1e);
		copy_from_vm(emu, buf2, u, sizeof buf2 - 1);
		printf("product revision [0x%05x] = \"%s\"\n", u, buf2);
	}

	ml = vm_read_segofs16(emu, VBE_BUF + 0x0e);

	for(modes = 0; modes < sizeof modelist / sizeof *modelist; )
	{
		u = x86emu_read_word(emu, ml + 2 * modes);
		if(u == 0xffff)
			break;
		modelist[modes++] = u;
	}

	if (!mode)
		printf("%u video modes:\n", modes);

	emu = NULL;

	for(u = 0; u < modes; u++)
	{
		number = modelist[u];
		if(mode && number != mode)
			continue;

		x86emu_done(emu);
		emu = x86emu_clone(vm->emu);

		emu->x86.R_EAX = 0x4f01;
		emu->x86.R_EBX = 0;
		emu->x86.R_ECX = number;
		emu->x86.R_EDX = 0;
		emu->x86.R_EDI = VBE_BUF;

		err = vm_run(emu, &d);

		if(opt.verbose >= 1)
			printf("mode: %u === vbe get mode info (0x%04x): %s (time %li, eax 0x%x, err = 0x%x)\n",
				u,
				number,
				emu->x86.R_AX == 0x4f ? "ok" : "failed",
				d,
				emu->x86.R_EAX,
				err
			);

		if(err || emu->x86.R_AX != 0x4f)
		{
			printf("  0x%04x: no mode info\n", number);
			continue;
		}

		attributes = x86emu_read_word(emu, VBE_BUF + 0x00);

		width = x86emu_read_word(emu, VBE_BUF + 0x12);
		height = x86emu_read_word(emu, VBE_BUF + 0x14);
		bytes_p_line = x86emu_read_word(emu, VBE_BUF + 0x10);

		win_A_start = x86emu_read_word(emu, VBE_BUF + 0x08) << 4;
		win_B_start = x86emu_read_word(emu, VBE_BUF + 0x0a) << 4;

		win_A_attr = x86emu_read_byte(emu, VBE_BUF + 0x02);
		win_B_attr = x86emu_read_byte(emu, VBE_BUF + 0x03);

		win_gran = x86emu_read_word(emu, VBE_BUF + 0x04) << 10;
		win_size = x86emu_read_word(emu, VBE_BUF + 0x06) << 10;

		bpp = res_bpp = 0;

		switch(x86emu_read_byte(emu, VBE_BUF + 0x1b))
		{
			case 0:
				bpp = -1;
			break;

			case 1:
				bpp = 2;
			break;

			case 2:
				bpp = 1;
			break;

			case 3:
				bpp = 4;
			break;

			case 4:
				bpp = 8;
			break;

			case 6:
				bpp = x86emu_read_byte(emu, VBE_BUF + 0x1f) +
				x86emu_read_byte(emu, VBE_BUF + 0x21) +
				x86emu_read_byte(emu, VBE_BUF + 0x23);
				res_bpp = x86emu_read_byte(emu, VBE_BUF + 0x19) - bpp;
				if(res_bpp < 0)
					res_bpp = 0;
		}

		fb_start = version >= 0x0200 ? x86emu_read_dword(emu, VBE_BUF + 0x28) : 0;

		pixel_clock = version >= 0x0300 ? x86emu_read_dword(emu, VBE_BUF + 0x3e) : 0;

		if(bpp == -1u)
		{
			printf("  0x%04x[%02x]: %ux%u, text\n", number, attributes, width, height);
		}
		else
		{
			*s = 0;
			if(res_bpp)
				sprintf(s, "+%d", res_bpp);
				
			printf("  0x%04x[%02x]: %ux%u+%u, %u%s bpp",
				number, attributes, width, height, bytes_p_line, bpp, s
			);

			if(pixel_clock)
				printf(", max. %u MHz", pixel_clock/1000000);
			if(fb_start)
				printf(", fb: 0x%08x", fb_start);
			printf(", %04x.%x", win_A_start, win_A_attr);
			if(win_B_start || win_B_attr)
				printf("/%04x.%x", win_B_start, win_B_attr);
			printf(": %uk", win_size >> 10);
			if(win_gran != win_size)
				printf("/%uk", win_gran >> 10);
			printf("\n");
		}
	}

	emu = x86emu_done(emu);
}

void list_modes(vm_t *vm, unsigned mode)
{
	x86emu_t *emu = NULL;
	int err = 0;
	time_t d;

	if(opt.verbose >= 1)
		printf("=== running bios\n");

	emu = x86emu_clone(vm->emu);

	emu->x86.R_EAX = 0x4f00;
	emu->x86.R_EBX = 0;
	emu->x86.R_ECX = 0;
	emu->x86.R_EDX = 0;
	emu->x86.R_ES = 0;
	emu->x86.R_EDI = VBE_BUF;

	x86emu_write_dword(emu, VBE_BUF, 0x32454256);		// "VBE2"
	
	// Run
	err = vm_run(emu, &d);

	if(opt.verbose >= 1)
		printf("=== vbe get info: %s (time %li, eax 0x%x, err = 0x%x)\n",
			emu->x86.R_AX == 0x4f ? "ok" : "failed",
			d,
			emu->x86.R_EAX,
			err
		);

	if(!err && emu->x86.R_AX == 0x4f)
	{
		if(opt.verbose >= 1)
			printf("=== vbe info\n");

		print_vbe_info(vm, emu, mode);
	}
	else {
		printf("=== no vbe info\n");
	}

	x86emu_done(emu);
}

int x86test()
{
	vm_t *vm;

	vm = vm_new();
	
	opt.trace_flags = X86EMU_TRACE_DEFAULT;
	opt.dump_flags  = X86EMU_DUMP_DEFAULT;
	opt.verbose = 2;
	opt.bios_map_all = 1;
	opt.all_modes = 1;
	opt.mode_set = 0;

	if(!vm_prepare(vm))
	{
		vm_free(vm);
		return 1;
	}
	
	list_modes(vm, opt.mode_set ? opt.mode : 0);
	printf("saindo...\n");
	vm_free(vm);
	
	return 0;
}
