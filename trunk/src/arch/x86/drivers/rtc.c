#include <drivers/rtc.h>
#include <kernel/thread.h>
#include <system.h>


/* ISR do RTC: Acorda os processos que solicitaram a syscall sleep. */
void rtc_irq(){
	uint32_t save;

	save = inportb(CMOS_ADDR);		/* Salva o valor da porta 0x70. */
	outportb(CMOS_ADDR, 0x0C);		/* acknowledge da IRQ8. */
	inportb(CMOS_DATA);

	outportb(CMOS_ADDR, save);		/* Atualiza o valor da porta 0x70. */
}

/* Lê da CMOS um determinado registro. */
uint32_t read_cmos_bcd(unsigned reg){
	uint32_t high_digit, low_digit;

	outportb(CMOS_ADDR, (inportb(CMOS_ADDR) & 0x80) | (reg & 0x7F));
	high_digit = low_digit = inportb(CMOS_DATA);

	/* Converte BCD para binário. */
	high_digit >>= 4;

	high_digit	&= 0x0F;
	low_digit	&= 0x0F;

	return (10*high_digit) + low_digit;
}

/* Pega o horario armazenado na CMOS.
   Formato: Cada unidade representa 1 segundo. */
uint32_t get_time(){
	uint32_t time;

	time  = read_cmos_bcd(0);
	time += read_cmos_bcd(2)*60;
	time += read_cmos_bcd(4)*(60*60);

	return time;
}

/* Pega a data armazenada na CMOS.
   Formato(bytes): YYMD */
uint32_t get_date(){
	uint32_t date;

	date  = read_cmos_bcd(9);
	date += read_cmos_bcd(8)*31;
	date += read_cmos_bcd(7)*(31*12);

	return date;
}

