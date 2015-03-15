/**
 * @file
 * @author  Mauro Joel Schutz
 * @version 0.1
 * @date 07/08/2012
 *
 * @section DESCRIPTION
 *
 * Arquivo com função mktime.
 *
 * @section LICENSE
 *
 * Copyright 2008-2012 Mauro Joel Schütz
 *
 * This file is part of Projeto SOmBRA.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with this program. If not, see http://www.gnu.org/licenses/.
 *
*/

#include <time.h>
#include <x86-common/drivers/rtc.h>

time_t time(time_t *timer)
{
	struct tm tim = { 0 };
	
	tim.tm_sec = read_cmos_bcd(RTC_CMOS_SECONDS);
	tim.tm_min = read_cmos_bcd(RTC_CMOS_MINUTES);
	tim.tm_hour = read_cmos_bcd(RTC_CMOS_HOURS);
	
	tim.tm_mday = read_cmos_bcd(RTC_CMOS_DAYOFMONTH);
	tim.tm_mon = read_cmos_bcd(RTC_CMOS_MONTH)-1;
	tim.tm_year = read_cmos_bcd(RTC_CMOS_YEAR);
	
	// Só iremos ter problemas com isso depois de 2069...
	if (tim.tm_year < 70)
		tim.tm_year += 100;
		
	time_t res = mktime(&tim);
		
	if (timer)
		*timer = res;
		
	return res;
}
