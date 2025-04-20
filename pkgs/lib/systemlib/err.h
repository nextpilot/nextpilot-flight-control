#ifndef _SYSTEMLIB_ERR_H
#define _SYSTEMLIB_ERR_H

#include <px4_platform_common/log.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>

#define warn(...)  PX4_WARN(__VA_ARGS__)
#define warnx(...) PX4_WARN(__VA_ARGS__)


#endif //_SYSTEMLIB_ERR_H
