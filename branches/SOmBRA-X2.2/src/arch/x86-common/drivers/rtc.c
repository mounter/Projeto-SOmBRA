#include <x86-common/drivers/rtc.h>
#include <system.h>


/* ISR do RTC: Acorda os processos que solicitaram a syscall sleep. */
void rtc_irq(){
	uint32_t save;

	save = inportb(CMOS_ADDR);		/* Salva o valor da porta 0x70. */
	outportb(CMOS_ADDR, 0x0C);		/* acknowledge da IRQ8. */
	(void)inportb(CMOS_DATA);

	outportb(CMOS_ADDR, save);		/* Atualiza o valor da porta 0x70. */
}

/* Lê da CMOS um determinado registro. */
uint32_t read_cmos_bcd(unsigned reg)
{
	uint32_t high_digit, low_digit;

	outportb(CMOS_ADDR, (inportb(CMOS_ADDR) & 0x80) | (reg & 0x7F));
	high_digit = low_digit = inportb(CMOS_DATA);

	/* Converte BCD para binário. */
	high_digit >>= 4;

	high_digit	&= 0x0F;
	low_digit	&= 0x0F;

	return (10*high_digit) + low_digit;
}
