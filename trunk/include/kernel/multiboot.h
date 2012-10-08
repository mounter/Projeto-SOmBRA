#ifndef _KERNEL_MULTIBOOT_H
#define _KERNEL_MULTIBOOT_H

// O número mágico para o cabeçalho Multiboot.
#define MULTIBOOT_HEADER_MAGIC          0x1BADB002

// As flags para o cabeçalho Multiboot.
#ifdef __ELF__
#   define MULTIBOOT_HEADER_FLAGS         0x00000003
#else
#   define MULTIBOOT_HEADER_FLAGS         0x00010003
#endif

// Constantes para decodificar o campo flags da estrutura mboot_info_t.
#define MBF_MEMORY		0x00
#define MBF_ROOTDEV		0x01
#define MBF_CMDLINE		0x02
#define MBF_MODS		0x03
#define MBF_AOUT_SYMS	0x04
#define MBF_ELF_SHDR	0x05
#define MBF_MEM_MAP		0x06
#define MBF_DRIVE_INFO	0x07
#define MBF_CFG_TABLE	0x08
#define MBF_LOADER_NAME	0x09
#define MBF_APM_TABLE	0x0A
#define MBF_VBE_INFO	0x0B

// O número mágico passado pelo boot loader Multiboot-compliant.
#define MULTIBOOT_BOOTLOADER_MAGIC      0x2BADB002

// O tamanho de nossa pilha (16KB).
#define STACK_SIZE                      0x4000

// C symbol format. HAVE_ASM_USCORE is defined by configure. */
#ifdef HAVE_ASM_USCORE
#   define EXT_C(sym)                     _ ## sym
#else
#   define EXT_C(sym)                     sym
#endif

#ifndef __ASSEMBLER__

#include "types.h"

// Tipos.

// O cabeçalho Multiboot.
typedef struct multiboot_header
{
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
} multiboot_header_t;

// A tabela de simbolos para a.out.
typedef struct aout_symbol_table
{
    uint32_t tabsize;
    uint32_t strsize;
    uint32_t addr;
    uint32_t reserved;
} aout_symbol_table_t;

// Tabela de seções para ELF.
typedef struct elf_section_header_table
{
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
} elf_section_header_table_t;

// As informações Multiboot.
typedef struct multiboot_info
{
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    union
    {
        aout_symbol_table_t aout_sym;
        elf_section_header_table_t elf_sec;
    } u;
    uint32_t mmap_length;
    uint32_t mmap_addr;
} multiboot_info_t;

// A estrutura para modulos
typedef struct module
{
    uint32_t mod_start;
    uint32_t mod_end;
    uint32_t string;
    uint32_t reserved;
} module_t;

// O mapa de memória. Be careful that the offset 0 is base_addr_low but no size.
typedef struct memory_map
{
    uint32_t size;
    uint32_t base_addr_low;
    uint32_t base_addr_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
} memory_map_t;

// Checa se o bit BIT nas FLAGS está atribuido
#define CHECK_FLAG(multiboot,bit)   ((multiboot->flags) & (1 << (bit)))

#endif


#endif
