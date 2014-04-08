#include <stdio.h>
#include <log.h>

int cvr_print(int level, const char *file, int line, const char *func, char *fmt, ...)
{
    char buf[2048];
    va_list vl;
    va_start(vl, fmt);
    vsnprintf(buf, sizeof(buf), fmt, vl);
    fprintf(stdout, "%s:%d (%s) %s", file,line,func,buf);
    va_end(vl);
    return 0;
}
