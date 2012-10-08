#include <x86/debug.h>
#include <udis86.h>
#include <stdio.h>

/* from START.ASM */
extern unsigned char tss_iopb[];
/*****************************************************************************
*****************************************************************************/
#define BPERL		16	/* byte/line for dump */

void dump_panic(uregs_t *regs, const char *fmt, ...)
{
	set_con(get_con());
	//clscr();
	va_list args;
	va_start(args, fmt);

	disable();
	set_color(CRED, CBLACK);
	puts("\nPANIC: ");

	set_color(CBRIGHTWHITE, CBLACK);

	(void)do_printf(fmt, args, kprintf_help, NULL, 0);

	va_end(args);

	printf("\n\nFazendo dump dos registradores:\n\n");
	dump_uregs(regs);

	printf("\n\nFazendo dump do kernel:\n");
	dump_eip(regs->eip, -11, 10);

	while(1){
		asm("cli\n"
            "hlt\n");
	}
}

void dump(void *data_p, unsigned count)
{
	unsigned char *data = (unsigned char *)data_p;
	unsigned byte1, byte2;

	while(count != 0)
	{
		for(byte1 = 0; byte1 < BPERL; byte1++)
		{
			if(count == 0)
				break;
			printf("%02X ", data[byte1]);
			count--;
		}
		printf("\t");
		for(byte2 = 0; byte2 < byte1; byte2++)
		{
			if(data[byte2] < ' ')
				printf(".");
			else
				printf("%c", data[byte2]);
		}
		printf("\n");
		data += BPERL;
	}
}
/*****************************************************************************
*****************************************************************************/
void dump_iopb(void)
{
	unsigned i, offset, mask;

/* dump IOPB only if it was initially all-ones */
	if(tss_iopb[127] != 0xFF)
		return;
	printf("The following I/O ports were accessed:\n");
	for(i = 0; i < 1024; i++)
	{
		offset = i >> 3;
		mask = 0x01 << (i & 7);
		if((tss_iopb[offset] & mask) == 0)
			printf("%03X ", i);
	}
	printf("\n");
}
/*****************************************************************************
*****************************************************************************/
void dump_uregs(uregs_t *regs)
{
	printf("EDI=%08X    ESI=%08X    EBP=%08X    ESP=%08X    EBX=%08X\n",
		regs->edi, regs->esi, regs->ebp, regs->esp, regs->ebx);
	printf("EDX=%08X    ECX=%08X    EAX=%08X     DS=%08X     ES=%08X\n",
		regs->edx, regs->ecx, regs->eax, regs->ds, regs->es);
	printf(" FS=%08X     GS=%08X intnum=%08X  error=%08X    EIP=%08X\n",
		regs->fs, regs->gs, regs->which_int, regs->err_code,
		regs->eip);
	printf(" CS=%08X EFLAGS=%08X", regs->cs, regs->eflags);
//	if((regs->eflags & 0x20000uL) || (regs->cs & 0x03))
		printf("   uESP=%08X    uSS=%08X",
			regs->user_esp, regs->user_ss);
	printf("\n");
//	if(regs->eflags & 0x20000uL)
		printf("vES=%04X    vDS=%04X    vFS=%04X    vGS=%04X\n",
		regs->v_es & 0xFFFF, regs->v_ds & 0xFFFF,
		regs->v_fs & 0xFFFF, regs->v_gs & 0xFFFF);
}

//static uint64_t o_skip = 0;
//static uint64_t o_count = 0;
//static unsigned char o_do_count= 0;
//static unsigned char o_do_off = 1;
//static unsigned char o_do_hex = 1;
//static unsigned char o_do_x = 0;
static unsigned o_vendor = UD_VENDOR_AMD;

#if defined(__amd64__) || defined(__x86_64__)
#  define FMT "l"
#else
#  define FMT "ll"
#endif

static unsigned ud_eip = 0x100000;
static int	ud_stop = 0;

int input_hook_eip(ud_t* u);

void dump_eip(unsigned eip, int skip, unsigned inst_count)
{
	ud_t ud_obj;

	ud_eip = eip+skip;
	ud_stop = inst_count;

	ud_init(&ud_obj);
	ud_set_mode(&ud_obj, 32);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);
	ud_set_vendor(&ud_obj, o_vendor);

	ud_set_pc(&ud_obj, ud_eip);
	ud_set_input_hook(&ud_obj, input_hook_eip);

	/* disassembly loop */
	while (ud_disassemble(&ud_obj)) {
//		if (o_do_off)
			printf("%08" FMT "x ", ud_insn_off(&ud_obj));
//		if (o_do_hex)
//		{
//			char* hex1, *hex2;
//			char c;
//			hex1 = ud_insn_hex(&ud_obj);
//			hex2 = hex1 + 16;
//			c = hex1[16];
//			hex1[16] = 0;
//			printf("%-16s %-24s", hex1, ud_insn_asm(&ud_obj));
//			hex1[16] = c;
//			if (strlen(hex1) > 16) {
//				printf("\n");
//				if (o_do_off)
//					printf("%15s -", "");
//				printf("%-16s", hex2);
//			}
//		} 
//		else
			printf(" %-24s", ud_insn_asm(&ud_obj));

		printf("\n");
		
		if(ud_stop)
			ud_stop--;
		else break;
  	}
}

int input_hook_eip(ud_t* u)
{
  int c;

//  if (o_do_count) {
//	  if (! o_count) {
//		return -1;
//	  } else o_count -- ;
//  }

  if (!ud_stop)
	return UD_EOI;

  c = *(unsigned char*)ud_eip;
  ud_eip++;

  return c;
}
