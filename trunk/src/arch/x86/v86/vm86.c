#include <x86/v86.h>
#include <stdio.h>
#include <x86/debug.h>

/* from START.ASM */
extern unsigned char tss_iopb[];
/*****************************************************************************
*****************************************************************************/
static unsigned long to_linear(unsigned seg, unsigned off)
{
	return (seg & 0xFFFF) * 16L + off;
}
/*****************************************************************************
*****************************************************************************/
unsigned peekb(unsigned seg, unsigned off)
{
	return *(uint8_t *)(to_linear(seg, off));
}
/*****************************************************************************
*****************************************************************************/
unsigned peekw(unsigned seg, unsigned off)
{
	return *(uint16_t *)(to_linear(seg, off));
}
/*****************************************************************************
*****************************************************************************/
unsigned long peekl(unsigned seg, unsigned off)
{
	return *(uint32_t *)(to_linear(seg, off));
}
/*****************************************************************************
*****************************************************************************/
void pokeb(unsigned seg, unsigned off, unsigned val)
{
	*(uint8_t *)(to_linear(seg, off)) = val;
}
/*****************************************************************************
*****************************************************************************/
void pokew(unsigned seg, unsigned off, unsigned val)
{
	*(uint16_t *)(to_linear(seg, off)) = val;
}
/*****************************************************************************
*****************************************************************************/
void pokel(unsigned seg, unsigned off, unsigned long val)
{
	*(uint32_t *)(to_linear(seg, off)) = val;
}
/*****************************************************************************
*****************************************************************************/
static void v86_enable_port(unsigned port)
{
	unsigned mask;

	mask = 0x01 << (port & 7);
	port >>= 3;
	tss_iopb[port] &= ~mask;
}
/*****************************************************************************
xxx - GPF if EIP > 0xFFFF ?
*****************************************************************************/
static unsigned v86_fetch8(uregs_t *regs)
{
	unsigned byte;

	byte = peekb(regs->cs, regs->eip);
	regs->eip = (regs->eip + 1) & 0xFFFF;
	return byte;
}
/*****************************************************************************
xxx - next four functions should fault (stack fault; exception 0Ch)
if stack straddles 0xFFFF
*****************************************************************************/
static void v86_push16(uregs_t *regs, unsigned value)
{
	regs->user_esp = (regs->user_esp - 2) & 0xFFFF;
	pokew(regs->user_ss, regs->user_esp, value);
}
/*****************************************************************************
*****************************************************************************/
static unsigned v86_pop16(uregs_t *regs)
{
	unsigned rv;

	rv = peekw(regs->user_ss, regs->user_esp);
	regs->user_esp = (regs->user_esp + 2) & 0xFFFF;
	return rv;
}
/*****************************************************************************
*****************************************************************************/
static void v86_push32(uregs_t *regs, unsigned long value)
{
	regs->user_esp = (regs->user_esp - 4) & 0xFFFF;
	pokel(regs->user_ss, regs->user_esp, value);
}
/*****************************************************************************
*****************************************************************************/
static unsigned long v86_pop32(uregs_t *regs)
{
	unsigned long rv;

	rv = peekl(regs->user_ss, regs->user_esp);
	regs->user_esp = (regs->user_esp + 4) & 0xFFFF;
	return rv;
}
/*****************************************************************************
*****************************************************************************/
void v86_int(uregs_t *regs, unsigned int_num)
{
/* push return IP, CS, and FLAGS onto V86 mode stack */
	v86_push16(regs, regs->eflags);
	v86_push16(regs, regs->cs);
	v86_push16(regs, regs->eip);
/* disable interrupts */
	regs->eflags &= ~0x200;
/* load new CS and IP from IVT */
	int_num *= 4;
	regs->eip = (regs->eip & ~0xFFFF) | peekw(0, int_num + 0);
	regs->cs = peekw(0, int_num + 2);
}
/*****************************************************************************
*****************************************************************************/
#define	PFX_ES		0x001
#define	PFX_CS		0x002
#define	PFX_SS		0x004
#define	PFX_DS		0x008
#define	PFX_FS		0x010
#define	PFX_GS		0x020

#define	PFX_OP32	0x040
#define	PFX_ADR32	0x080
#define	PFX_LOCK	0x100
#define	PFX_REPNE	0x200
#define	PFX_REP		0x400

int v86_emulate(uregs_t *regs)
{
	unsigned init_eip, prefix, i;

/* save current EIP so we can re-try instructions
instead of skipping over or emulating them */
	init_eip = regs->eip;
/* consume prefix bytes */
	prefix = 0;
	while(1)
	{
		i = v86_fetch8(regs);
		switch(i)
		{
		case 0x26:
			prefix |= PFX_ES;
			break;
		case 0x2E:
			prefix |= PFX_CS;
			break;
		case 0x36:
			prefix |= PFX_SS;
			break;
		case 0x3E:
			prefix |= PFX_DS;
			break;
		case 0x64:
			prefix |= PFX_FS;
			break;
		case 0x65:
			prefix |= PFX_GS;
			break;
		case 0x66:
			prefix |= PFX_OP32;
			break;
		case 0x67:
			prefix |= PFX_ADR32;
			break;
		case 0xF0:
			prefix |= PFX_LOCK;
			break;
		case 0xF2:
			prefix |= PFX_REPNE;
			break;
		case 0xF3:
			prefix |= PFX_REP;
			break;
		default:
			goto END;
		}
	}
END:
	switch(i)
	{
/* PUSHF */
	case 0x9C:
		if(prefix & PFX_OP32)
			v86_push32(regs, regs->eflags);
		else
			v86_push16(regs, regs->eflags);
		return 0;
/* POPF */
	case 0x9D:
		if(prefix & PFX_OP32)
		{
			if(regs->user_esp > 0xFFFC)
				return +1;
			regs->eflags = v86_pop32(regs);
		}
		else
		{
			if(regs->user_esp > 0xFFFE)
				return +1;
/* tarnation!		regs->eflags = v86_pop16(regs); */
			regs->eflags = (regs->eflags & 0xFFFF0000L) |
				v86_pop16(regs);
		}
		return 0;
/* INT nn */
	case 0xCD:
		i = v86_fetch8(regs); /* get interrupt number */
		v86_int(regs, i);
		return 0;
/* IRET */
	case 0xCF:
/* pop (E)IP, CS, (E)FLAGS */
		if(prefix & PFX_OP32)
		{
			if(regs->user_esp > 0xFFF4)
				return +1;
			regs->eip = v86_pop32(regs);
			regs->cs = v86_pop32(regs);
			regs->eflags = v86_pop32(regs);
		}
		else
		{
			if(regs->user_esp > 0xFFFA)
				return +1;
			regs->eip = v86_pop16(regs);
			regs->cs = v86_pop16(regs);
			regs->eflags = (regs->eflags & 0xFFFF0000L) |
				v86_pop16(regs);
		}
		return 0;
/************************************
I/O functions are not (yet) emulated. We just enable the
appropriate port in the IOPB and retry the instruction.
**************************************/
/* IN AL,imm8 */
	case 0xE4:
/* OUT imm8,AL */
	case 0xE6:
		i = v86_fetch8(regs);
		v86_enable_port(i);
/* restore original EIP -- we will re-try the instruction */
		regs->eip = init_eip;
		return 0;
/* IN [E]AX,imm8 */
	case 0xE5:
/* OUT imm8,[E]AX */
	case 0xE7:
		i = v86_fetch8(regs);
		v86_enable_port(i);
		v86_enable_port(i + 1);
		if(prefix & PFX_OP32)
		{
			v86_enable_port(i + 2);
			v86_enable_port(i + 3);
		}
		regs->eip = init_eip;
		return 0;
/* INSB */
	case 0x6C:
/* OUTSB */
	case 0x6E:
/* IN AL,DX */
	case 0xEC:
/* OUT DX,AL */
	case 0xEE:
		i = regs->edx & 0xFFFF;
		v86_enable_port(i);
		regs->eip = init_eip;
		return 0;
/* INSW, INSD */
	case 0x6D:
/* OUTSW, OUTSD */
	case 0x6F:
/* IN [E]AX,DX */
	case 0xED:
/* OUT DX,[E]AX */
	case 0xEF:
		i = regs->edx & 0xFFFF;
		v86_enable_port(i);
		v86_enable_port(i + 1);
		if(prefix & PFX_OP32)
		{
			v86_enable_port(i + 2);
			v86_enable_port(i + 3);
		}
		regs->eip = init_eip;
		return 0;
/* CLI */
	case 0xFA:
		regs->eflags &= ~0x200;
		return 0;
/* STI */
	case 0xFB:
		regs->eflags |= 0x200;
		return 0;
	}
/* anything else */
	printf("Error in V86 mode at CS:IP=%04X:%04X\n",
		regs->cs, init_eip);
	printf("Dump of bytes at CS:EIP:\n");
	dump((void *)(to_linear(regs->cs, init_eip)), 16);
	return -1;
}

