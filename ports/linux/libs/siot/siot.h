#ifndef __SIOT_H__
#define __SIOT_H__

#include <stdarg.h>

#include "siot-security.h"
#include "siot-transport.h"

#define DEBUG 1

#if DEBUG
static inline void debug(const char* format, ...)
{
        va_list arglist;

        va_start(arglist, format);
        vprintf(format, arglist);
        va_end(arglist);
}
#else
static inline void debug(const char* format, ...){}
#endif

#endif
