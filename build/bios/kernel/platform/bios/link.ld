
OUTPUT_FORMAT(elf64-x86-64)
ENTRY(SOmBRA)
KERNEL_VMA = 0x200000;
SECTIONS
{
 .text KERNEL_VMA : AT(KERNEL_VMA)
 {
  g_d_code = .; _g_d_code = .;
  *(.dtext)
  . = ALIGN(4096);

  g_code = .; _g_code = .;
  *(.text)
  *(.rodata*)
  . = ALIGN(4096);
 }

 .data : AT(KERNEL_VMA + (g_d_data - g_d_code))
 {
  g_d_data = .; _g_d_data = .;
  *(.ddata)
  . = ALIGN(4096);

  g_data = .; _g_data = .;
  *(.data)
  . = ALIGN(4096);
 }

 .ehframe : AT(KERNEL_VMA + (_ehframe - g_d_code))
 {
  _ehframe = .;
  *(.ehframe)
  . = ALIGN(4096);
 }

 .bss : AT(KERNEL_VMA + (g_d_bss - g_d_code))
 {
  g_d_bss = .; _g_d_bss = .;
  *(.dbss)
  . = ALIGN(4096);

  g_bss = .; _g_bss = .;
  *(.bss)






  *(COMMON)
  . = ALIGN(4096);

  g_bss_end = .; _g_bss_end = .;
 }

 g_end = .; _g_end = .;

 /DISCARD/ :
 {
  *(.comment)
 }
}
