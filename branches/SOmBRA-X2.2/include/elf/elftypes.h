/*************************************************
** Autor: Mauro Joel Schütz
** DOMÍNIO PÚBLICO: 
**	VOCÊ PODE UTILIZAR ESTES ARQUIVOS DA FORMA QUE BEM QUISER,
**	SOMENTE MANTENHA O NOME DO AUTOR.
*************************************************/
#ifndef _ELFTYPES_H_
#define _ELFTYPES_H_

#include <stdint.h>

typedef uint64_t Elf_Xword;
typedef int64_t Elf_Sxword;
typedef uint32_t Elf_Word;
typedef int32_t Elf_Sword;
typedef uint16_t Elf_Half;

typedef uint32_t Elf_Addr32;
typedef uint32_t Elf_Off32;

typedef uint64_t Elf_Addr64;
typedef uint64_t Elf_Off64;


#endif
