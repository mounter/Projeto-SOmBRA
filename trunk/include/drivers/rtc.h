#ifndef RTC_H
#define RTC_H

#include <kernel.h>

void rtc_irq();
uint32_t get_time();
uint32_t get_date();

#endif
