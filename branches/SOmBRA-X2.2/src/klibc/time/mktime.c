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

#define EPOCH 70
#define _IS_LEAP(year) ((year%4==0 && year%100!=0) || year%400==0)

// por enquanto sem horario de verão e sem suporte a utc
time_t mktime(struct tm* timer)
{
	int monthdays[12] = { 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
	int cyear = timer->tm_year-EPOCH;
	int days = ((cyear/4) * 366)+((cyear-cyear/4)*365);
	
	timer->tm_yday = timer->tm_mday;
	if (timer->tm_mon > 0)
		timer->tm_yday += monthdays[timer->tm_mon-1];
		
	int year = (timer->tm_year+1900);
	
	if (_IS_LEAP(year) && timer->tm_mon > 1)
		timer->tm_yday++;
		
	int i;
	for (i = 1900; i < year; i += 100)
	{
		if (i >= (1900+EPOCH) && !_IS_LEAP(i))
			timer->tm_yday--;
	}
	
	days += timer->tm_yday-1;
	
	if ((timer->tm_wday = (days + 4) % 7) < 0)
		timer->tm_wday += 7;
	
	return (days * 86400)+timer->tm_sec+timer->tm_min*60+timer->tm_hour*3600;
}
