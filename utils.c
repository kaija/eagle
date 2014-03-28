#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int read_proc(char *proc, char *buf, int len)
{
    int ret = -1;
    if(!proc || !buf) return -1;
    FILE *fp = NULL;
    fp = fopen(proc, "r");
    if(fp) {
        if(fgets(buf, len, fp) != NULL) {
            ret = 0;
        }
        fclose(fp);
    }
    return ret;
}
