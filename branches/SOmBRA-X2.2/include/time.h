#ifndef _TIME_H
#define _TIME_H

#ifndef __TIME_T_DEFINED
#define __TIME_T_DEFINED
#	ifdef __x86_64__
	typedef long	__time_t;
#	else
	typedef long long __time_t;
#	endif
#endif

typedef __time_t time_t;

#ifndef _TM_DEFINED
#define _TM_DEFINED
  struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
  };
#endif

#include "_null.h"

time_t     mktime(struct tm *);
time_t time(time_t *tloc);

#endif
