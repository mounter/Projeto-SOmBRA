#ifndef FLOPPY_H
#define FLOPPY_H

#include <kernel.h>

// Para  floopy 1.44MB
#define FDC_SECTORS	18	// Sectors per track.
#define FDC_HEADS	2	// Heads per track.
#define FDC_TRACKS	80	// Tracks per disk.
#define FDC_SECTOR_SIZE	512	// Bytes per sector.

// Porta de I/O para o floppy
#define FDC_DOR		0x3F2   // Digital Output Register.
#define FDC_MSR		0x3F4   // Main Status Register (read).
#define FDC_DSR		0x3F4   // Data Rate Select Register (write).
#define FDC_DATA	0x3F5   // Data Register.
#define FDC_DIR		0x3F7   // Digital Input Register.
#define FDC_CCR		0x3F7   // Configuration Control Register.

// Comandos para o floppy
#define CMD_SPECIFY	0x03	// Specify.
#define CMD_WRITE	0xC5	// Write.
#define CMD_READ	0xE6	// Read.
#define CMD_RECAL	0x07	// Recalibrate.
#define CMD_SENSEI	0x08	// Send a "sense interrupt status".
#define CMD_FORMAT	0x4D	// Format a track.
#define CMD_SEEK	0x0F	// Seek a track.
#define CMD_VERSION	0x10	// Get the controller version.

#define MSR_BUSY	0x10    // O controlador está ocupado.

#define FDC_TIME_MOTOR_SPINUP	500  // FDC motor spin-up time (msec).
#define FDC_TIME_MOTOR_OFF	3000 // FDC timeout to turn the motor off (msec).


typedef struct{
  unsigned int 	size,   // # of sectors total.
		spt,    // sectors per track.
		heads,  // # of heads.
		tracks; // # of tracks.
  unsigned char	fmtgap, // gap3 while formatting.
		rwgap,  // gap3 while reading/writing.
		rate;   // data rate.
  char         *name;   // format name.
} floppy_struct;

void floppy_irq();
bool floppy_init();
bool floppy_down();
void floppy_thread();
bool fdc_write(int block, unsigned char *buffer, unsigned int count);
bool fdc_read(int block, unsigned char *buffer, unsigned int count);

#endif
