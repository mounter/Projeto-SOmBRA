#ifndef _STDBOOL_H_
#define _STDBOOL_H_

#define __bool_true_false_are_defined   1

#ifndef __cplusplus

#define false   0
#define true    1

#define bool    _Bool

#if __STDC_VERSION__ < 199901L && __GNUC__ < 3
typedef int     _Bool;
#endif

#endif

#endif

