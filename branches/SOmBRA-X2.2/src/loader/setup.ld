/* let the linker use its 'native' format (ELF/COFF/PE) */
OUTPUT_FORMAT("elf32-i386")
/* no leading underscores for symbols handled in asm: */
ENTRY(SOmBRA)
load_adr = 0x100000; /* 1 meg = physical address */
virt_adr = 0xC0000000;
SECTIONS{
  .text load_adr : AT(load_adr){
/* discardable kernel code */
	g_d_code = .; _g_d_code = .;
	*(.dtext*)
	. = ALIGN(4096);
/* kernel code */
	g_code = .; _g_code = .;
	*(.text*)
	*(.rodata*)
	. = ALIGN(4096);
    }
    .data : AT(load_adr + (g_d_data - g_d_code))
    {
/* discardable kernel data */
	g_d_data = .; _g_d_data = .;
	*(.ddata*)
	. = ALIGN(4096);
/* kernel data */
	g_data = .; _g_data = .;
	*(.data*)
	. = ALIGN(4096);
    }
    .bss : AT(load_adr + (g_d_bss - g_d_code))
    {
/* discardable kernel BSS */
	g_d_bss = .; _g_d_bss = .;
	*(.dbss*)
	. = ALIGN(4096);
/* kernel BSS */
	g_bss = .; _g_bss = .;
	*(.bss*)
	*(COMMON) /* "common" variables */
	. = ALIGN(4096);
	g_bss_end = .; _g_bss_end = .;
    }
/* end of kernel */
    g_end = .; _g_end = .;
}
