#ifndef RTC_H
#define RTC_H

#include <time.h>
#include <stdint.h>

#define CMOS_ADDR	0x70
#define CMOS_DATA	0x71

#define RTC_CMOS_SECONDS	0x00	// SEGUNDOS
#define RTC_CMOS_MINUTES	0x02	// MINUTOS
#define RTC_CMOS_HOURS		0x04	// HORAS
#define RTC_CMOS_WEEKDAY	0x06	// DIA DA SEMANA
#define RTC_CMOS_DAYOFMONTH	0x07	// DIA DO MÊS
#define RTC_CMOS_MONTH		0x08	// MÊS
#define RTC_CMOS_YEAR		0x09	// ANO
#define RTC_CMOS_CENTURY	0x32	// 19YY ou 20YY ? (Pode não existir)
#define RTC_CMOS_SRA		0x0A    // Registrador de Estado A
#define RTC_CMOS_SRB		0x0B	// Registrador de Estado B

int rtc_cmos_update_in_progress();
unsigned char rtc_read_cmos(int reg);

#define RTC_WAIT_UPDATE() while(rtc_cmos_update_in_progress())

void rtc_irq();
uint32_t read_cmos_bcd(unsigned reg);

#endif
