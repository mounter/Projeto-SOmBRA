// Problemas com o udis86, verificar!
#include <x86_64/debug.h>
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
	dump_rip(regs->rip, -11, 10);

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

	printf("RDI=%016lX    RSI=%016lX    RBP=%016lX\n",
		regs->rdi, regs->rsi, regs->rbp);
	printf("R15=%016lX    R14=%016lX    R13=%016lX\n",
		regs->r15, regs->r14, regs->r13);	
	printf("R12=%016lX    R11=%016lX    R10=%016lX\n",
		regs->r12, regs->r11, regs->r10);
	printf("R9=%016lX     R8=%016lX    RBX=%016lX\n",
		regs->r9, regs->r8, regs->rbx);
	printf("RDX=%016lX    RCX=%016lX    RAX=%016lX\n",
		regs->rdx, regs->rcx, regs->rax);
	printf(" FS=%016lX     GS=%016lX  intnum=%016lX\n",
		regs->fs, regs->gs, regs->which_int);
	printf("error=%016lX    RIP=%016lX    CS=%016lX\n",
		regs->err_code, regs->rip, regs->cs);
	printf("RFLAGS=%016lX    RSP=%016lX    SS=%016lX\n",
		regs->rflags, regs->rsp, regs->ss);
	printf("\n");
}

static unsigned o_vendor = UD_VENDOR_AMD;

#if defined(__amd64__) || defined(__x86_64__)
#  define FMT "l"
#else
#  define FMT "ll"
#endif

static uint64_t ud_rip = 0x200000;
static int	ud_stop = 0;

int input_hook_rip(ud_t* u);

void dump_rip(rregs_t rip, int skip, unsigned inst_count)
{
	ud_t ud_obj;

	ud_rip = rip+skip;
	ud_stop = inst_count;

	ud_init(&ud_obj);
	ud_set_mode(&ud_obj, 64);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);
	ud_set_vendor(&ud_obj, o_vendor);

	ud_set_pc(&ud_obj, ud_rip);
	ud_set_input_hook(&ud_obj, input_hook_rip);


	/* disassembly loop */
	while (ud_disassemble(&ud_obj))
	{
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

int input_hook_rip(ud_t* u)
{
  int c;

  if (!ud_stop)
	return UD_EOI;

  c = *(unsigned char*)ud_rip;
  ud_rip++;

  return c;
}
