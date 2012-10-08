/*
PUBLIC DOMAIN CODE
USE IT AT YOUR OWN RISK!!!

AUTHOR: MAURO JOEL SCHÜTZ
*/
#ifndef _AR_H
#define _AR_H

#include <fs.h>

#define	ARMAG	"!<arch>\n"
#define	SARMAG	8
#define	ARFMAG	"`\n"

#pragma pack(1)
struct ar_hdr		/* archive file member header - printable ascii */
{
	char	ar_name[16];	/* file member name - `/' terminated */
	char	ar_date[12];	/* file member date - decimal */
	char	ar_uid[6];	/* file member user id - decimal */
	char	ar_gid[6];	/* file member group id - decimal */
	char	ar_mode[8];	/* file member mode - octal */
	char	ar_size[10];	/* file member size - decimal */
	char	ar_fmag[2];	/* ARFMAG - string to end header */
};

#define AR_MAGIC        0xAF

// Estrutura que representa o cabeçalho de um arquivo
typedef struct
{
   unsigned char magic; // The magic number is there to check for consistency.
   char name[16];
   unsigned int offset; // Offset in the initrd the file starts.
   unsigned int length; // Length of the file.
} ar_file_t;

typedef struct
{
    size_t node_count;
    uintptr_t *inodes;
} ar_header_t;

#endif	/* _AR_H */
