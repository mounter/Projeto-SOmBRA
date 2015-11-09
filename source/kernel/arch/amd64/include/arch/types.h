/**
 * Ficheiro com a definição dos tipos de dados
 * para a arquitetura x86
 */

#ifndef __ARCH_TYPES_H
#define __ARCH_TYPES_H

/** Defenição de alguns formatos para o printf() */
#ifdef CONFIG_64BIT
#   define PRIxPHYS	"lx"		/**< Formato para phys_ptr_t (hexadecimal). */
#   define PRIuPHYS	"lu"		/**< Formato para phys_ptr_t. */
#else
#   define PRIxPHYS	"llx"		/**< Formato para phys_ptr_t (hexadecimal). */
#   define PRIuPHYS	"llu"		/**< Formato para phys_ptr_t. */
#endif

/** Tipo inteiro que consegue representar um apontador */
typedef unsigned long ptr_t;

/** Tipos inteiros que conseguem representar um endereço fisico ou tamanho */
typedef uint64_t phys_ptr_t;
typedef uint64_t phys_size_t;

#endif // __ARCH_TYPES_H
