#include <floppy.h>
#include <dma.h>
#include <string.h>
#include <system.h>
#include <stdio.h>

volatile bool fdc_motor = false;     // Flag para o motor.
volatile int fdc_timeout = 0;        // Contador para o timeout.
volatile int fdc_motor_countdown=0;  // Contador para o motor do floppy
volatile bool fdc_done = false;      // Flag para indicar fim da operação.
volatile bool fdc_change = false;    // Flag para indicar alterações no disco.

volatile unsigned char fdc_status[7] = { 0 }; // Status do FDC.

volatile unsigned char fdc_track = 0xFF;      // Posição atual da cabeça do disco.
volatile unsigned char ST0 = 0;               // Status register 0.

// Tipos de floppy reconhecido pelo SO.
static floppy_struct floppy_type[] = {
  { 2880, 18, 2, 80, 0x54, 0x1B, 0x00, "H1440" },	// 1.44MB
  { 3360, 21, 2, 80, 0x0C, 0x1C, 0x00, "H1680" }	// 1.68MB
};

unsigned char  fdc_geometry = 0;     // Tipo atual do floppy(1.44MB).
unsigned char *fdc_buffer;           // Buffer de entrada e saída do floppy.


void delay(int n){
	int i, j, k;
	for (i=0;i<9999;i++){
		for (j=0;j<9999;j++){
			k = j + i;
		}
	}
}


// Envia um byte para o FDC
void fdc_sendbyte(unsigned char b){
  int msr;
  int tmo;

  for (tmo=0; tmo<128; tmo++){
    msr = inportb(FDC_MSR);
    if ((msr & 0xC0) == 0x80){
      outportb(FDC_DATA, b);
      return;
    }
    inportb(0x80);
  }
}

// Pega um byte do FDC
int fdc_getbyte(){
  int msr;
  int tmo;

  for (tmo=0; tmo<128; tmo++){
    msr = inportb(FDC_MSR);
    if ((msr & 0xD0) == 0xD0){
      return(inportb(FDC_DATA));
    }
    inportb(0x80);
  }
  return -1;
}

// Ativar o motor do floopy.
void fdc_motor_on(){
  if (!fdc_motor){
    outportb(FDC_DOR, 0x1C);
    delay(FDC_TIME_MOTOR_SPINUP);
    fdc_motor = true;
  }
  fdc_motor_countdown = -1;
}

void fdc_motor_off(){
  if (fdc_motor && (fdc_motor_countdown==-1)){
    fdc_motor_countdown = FDC_TIME_MOTOR_OFF/1000*HZ;
  }
}

// Espera pela finalização de uma operação do FDC.
bool fdc_wait(bool sensei){
  unsigned char i;

  fdc_timeout = HZ;

  // Espera por uma IRQ ou Timeout
  while((!fdc_done) && (fdc_timeout)){
    enable();
    disable();
  }

  // Lê o resultado do comando.
  i = 0;
  while ((i < 7) && (inportb(FDC_MSR) & MSR_BUSY)){
    fdc_status[i++] = fdc_getbyte();
  }

  // Envia o comando "sense interrupt status"
  if (sensei){
    fdc_sendbyte(CMD_SENSEI);
    ST0 = fdc_getbyte();
    fdc_track = fdc_getbyte();
  }

  // Se ocorrer timeout
  if (!fdc_done){
    // Verifica se houve alguma alteração no floppy.
    if (inportb(FDC_DIR) & 0x80){
      fdc_change = true;
    }
    return false;
  }
  else{
    fdc_done = false;
    return true;
  }

}

// Recalibra o floppy.
void fdc_recalibrate(){
  // Aciona o motor
  fdc_motor_on();

  // Envia o comando de recalibragem.
  fdc_sendbyte(CMD_RECAL);
  fdc_sendbyte(0);

  // Espera pelo comando de fim da recalibragem.
  fdc_wait(true);

  // Desliga o motor.
  fdc_motor_off();
}

//! \brief Seek a track.
//! \param track The track to seek.
//! \return
//!	\li #TRUE on success;
//!	\li #FALSE if an error occurs.
bool fdc_seek(int track){
	// If already threre return //
  if (fdc_track == track){
    return true;
  }

	// Turn the motor on //
  fdc_motor_on();

	// Send seek command //
  fdc_sendbyte(CMD_SEEK);
  fdc_sendbyte(0);
  fdc_sendbyte(track);

	// Wait until seek is finished //
  if (!fdc_wait(true)){
    // Timeout! //
    fdc_motor_off();
    return false;
  }

	// Let the head settle for 15msec //
  delay(15);

	// Turn off the motor //
  fdc_motor_off();

	// Check if seek worked //
  if ((ST0 != 0x20) || (fdc_track != track)){
    return false;
  }
  else{
    return true;
  }
}

//! \brief Reset the floppy disk controller.
void fdc_reset(){
	// Stop the motor and disable IRQ //
  outportb(FDC_DOR, 0x00);

	// Program data rate (500K/s) //
  outportb(FDC_DSR, 0x00);

	// re-enable floppy IRQ //
  outportb(FDC_DOR, 0x0C);

	// Wait for fdc //
  fdc_done = true;
  fdc_wait(true);

	// Specify drive timings //
  fdc_sendbyte(CMD_SPECIFY);
  fdc_sendbyte(0xDF); // SRT = 3ms, HUT = 240ms
  fdc_sendbyte(0x02); // HLT = 4ms, ND = 0 (DMA mode selected)

	// Clear disk change flag //
  fdc_seek(1);
  fdc_recalibrate();
  fdc_change = false;
}

//! \brief
//!	Convert from a LBA (Linear Block Address) address to a
//!	CHS (Cylinder, Head, Sector) coordinates.
//! \param lba The LBA address.
//! \param track A pointer to the track coordinate.
//! \param head A pointer to the head coordinate.
//! \param sector A pointer to the sector coordinate.
void lba2chs(int lba, int *track, int *head, int *sector){
  *track = lba / (floppy_type[fdc_geometry].spt * floppy_type[fdc_geometry].heads);
  *head = (lba / floppy_type[fdc_geometry].spt) % floppy_type[fdc_geometry].heads;
  *sector = (lba % floppy_type[fdc_geometry].spt) + 1;
}

//! \brief
//!	Perform a read/write operation with the floppy drive using
//!	the DMA (Direct Memory Access).
//! \param block The LBA address of the block on the floppy.
//! \param buffer
//!	The address of the buffer in memory for the transfer
//!	operation. It must have a size of at least #FDC_SECTOR_SIZE
//!	bytes.
//! \param do_read
//!	\li #TRUE to perform a read operation (from disk to memory);
//!	\li #FALSE to perform a write operation (from memory to disk).
//! \return
//!	\li #TRUE on success;
//!	\li #FALSE if an error occurs.
bool fdc_rw(int block, unsigned char *buffer, bool do_read){
  int track, head, sector, tries;

	// Translate the logical address into CHS address //
  lba2chs(block, &track, &head, &sector);

	// Spin up the motor //
  fdc_motor_on();

  if (!do_read){
		// Write operation => copy data from buffer into fdc_buffer //
    memcpy(fdc_buffer, buffer, FDC_SECTOR_SIZE);
  }

  for(tries=0; tries<3; tries++){
    // Check for disk changed //
    if (inportb(FDC_DIR) & 0x80){
      fdc_change = true;
      fdc_seek(1);
      fdc_recalibrate();
      fdc_motor_off();
      return false;
    }
		// Move the head to the right track //
    if (!fdc_seek(track)){
      fdc_motor_off();
      return false;
    }
		// Program data rate //
    outportb(FDC_CCR, floppy_type[fdc_geometry].rate);

		// Send command //
    if (do_read){
                // Read operation //
      dma_xfer(2, (unsigned int)fdc_buffer, FDC_SECTOR_SIZE, false);
      fdc_sendbyte(CMD_READ);
    }
    else{
			// Write operation //
      dma_xfer(2, (unsigned int)fdc_buffer, FDC_SECTOR_SIZE, true);
      fdc_sendbyte(CMD_WRITE);
    }

    fdc_sendbyte(head << 2);
    fdc_sendbyte(track);
    fdc_sendbyte(head);
    fdc_sendbyte(sector);
    fdc_sendbyte(2); // 512 bytes/sector //
    fdc_sendbyte(floppy_type[fdc_geometry].spt);
    fdc_sendbyte(floppy_type[fdc_geometry].rwgap);
    fdc_sendbyte(0xFF); // DTL = unused //

		// Wait for command completion //
    if (!fdc_wait(false)){
      return false; // Timed out! //
    }

    if ((fdc_status[0] & 0xC0) == 0) break; // Worked! :) //

		// Try again... //
    fdc_recalibrate();
  }

	// Stop the motor //
  fdc_motor_off();

  if (do_read){
		// Read operation => copy data from fdc_buffer into buffer //
    memcpy(buffer, fdc_buffer, FDC_SECTOR_SIZE);
  }

	// Return if the R/W operation terminates successfully //
  return(tries != 3);
}

//! \brief Read some contiguous blocks from the floppy disk.
//! \param block The starting block to read.
//! \param buffer
//!	The destination buffer in memory where the
//!	blocks will be copied.
//! \param count
//!	How many blocks to read.
//! \return
//!	\li #TRUE on success;
//!	\li #FALSE if an error occurs.
bool fdc_read(int block, unsigned char *buffer, unsigned int count){
  register int i;

  for(i=0; i<count; i++){
    if (!(fdc_rw(block+i, buffer+(FDC_SECTOR_SIZE*i), true))){
      return false;
    }
  }
	// Read operation OK! //
  return true;
}

//! \brief Write some contiguous blocks to the floppy disk.
//! \param block The starting block to write.
//! \param buffer
//!	The address in memory from which the data will be copied.
//! \param count
//!	How many blocks to write.
//! \return
//!	\li #TRUE on success;
//!	\li #FALSE if an error occurs.
bool fdc_write(int block, unsigned char *buffer, unsigned int count){
  register int i;

  for(i=0; i<count; i++){
    if (!(fdc_rw(block+i, buffer+(FDC_SECTOR_SIZE*i), false))){
      return false;
    }
  }
	// Write operation OK! //
  return true;
}

//! \brief Check if the floppy disk was changed.
//! \return
//!	\li #TRUE if the disk was changed;
//!	\li #FALSE otherwise.
bool fdc_is_changed(){
  return fdc_change;
}

// --- Interrupt routines --------------------------------------------- //

//! \brief
//!	This is a routine called from clock_thread() at every clock
//!	tick to perform the floppy motor kill countdown and to control
//!	the floppy timeouts.
void floppy_thread(){
  if (fdc_timeout>0){
    fdc_timeout--;
  }

  if (fdc_motor_countdown > 0){
    fdc_motor_countdown--;
  }
  else if (fdc_motor && !fdc_motor_countdown){
    outportb(FDC_DOR, 0x0C);
    fdc_motor = false;
  }

}

/** \ingroup Handlers */
//! \brief
//!	This is the floppy interrupt handler routine. It is invoked
//!	every time that a floppy operation successfully completes.
void floppy_irq(){
  fdc_done = true;
  outportb(0x20, 0x20);
}

// --- Initialization --------------------------------------------------//

//! \brief Initialize the floppy driver.
//! \return
//!	\li #TRUE on success;
//!	\li #FALSE if an error occurs.
bool floppy_init(){
  int v;

	// Create the FDC buffer //
  fdc_buffer = dma_phys_alloc();

	// Reset the controller //
  fdc_reset();

	// Get floppy controller version //
  fdc_sendbyte(CMD_VERSION);
  v = fdc_getbyte();

  switch (v){
    case 0xFF:
      return false;
    break;
    case 0x90:
      printf("   Enhanced controller foi encontrado!\n");
      return true;
    break;
    default:
      printf("   Controlador 8272A/765A foi encontrado! (cmd_version=%X)\n", v);
      return true;
    break;
  }
}

bool floppy_down()
{
	fdc_reset();
	dma_phys_free(fdc_buffer);
	fdc_motor_off();
	outportb(FDC_DOR, 0x0C);
	fdc_motor = false;
	return true;
}

