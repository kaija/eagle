#ifndef __LOG_H
#define __LOG_H

#include <stdarg.h>

#define LOG_DEBUG   0
#define LOG_INFO    1
#define LOG_WARN    2
#define LOG_ERROR   3
#define LOG_FATAL   4

#define LOG(level, fmt, args...)  cvr_print(level,__FILE__,__LINE__,__func__, fmt, ##args)
int cvr_print(int level, const char *file, int line, const char *func, char *fmt, ...);

#endif
