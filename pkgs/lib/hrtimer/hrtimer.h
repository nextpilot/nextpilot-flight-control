#include <rtconfig.h>

#if defined(PKG_USING_HRTIMER_V1)
#   include "hrtimer/V1/hrtimer.h"
#elif defined(PKG_USING_HRTIMER_V2)
#   include "hrtimer/V2/hrtimer.h"
#else
#   include "hrtimer/V3/hrtimer.h"
#endif
