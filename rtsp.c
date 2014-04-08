#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rtsp.h"

#define RTSP_PKT_SIZE  4096
static char rtsp_time[64];

void rtsp_get_time()
{
    memset(rtsp_time, 0, 64);
    struct tm *local;
    time_t t;

    t = time(NULL);
    local = localtime(&t);
    strftime(rtsp_time, 64,"%a, %b %d %Y %T GMT", local);
    //printf("%s\n", rtsp_time);
    //printf("Local time and date: %s\n", asctime(local));
    //local = gmtime(&t);
    //strftime(rtsp_time, 64,"%a, %b %d %Y %T GMT", local);
    //printf("%s\n", rtsp_time);
    //printf("UTC time and date: %s\n", asctime(local));
}

int rtsp_resp_gen(int type, char **pkt, int cseq)
{
    char *buf = malloc(RTSP_PKT_SIZE);
    *pkt = buf;
    int len = 0;
    if(type == RTSP_RESP_OPTIONS) {
        len = snprintf(buf, RTSP_PKT_SIZE, "RTSP/%s 200 OK\r\nCSeq: %d\r\nDate: %s\r\n", RTSP_VER, cseq, rtsp_time);
    }
#if 0
    RTSP/1.0 200 OK

CSeq: 2

Date: Thu, Jan 01 2009 22:24:13 GMT

Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, SET_PARAMETER
#endif
    return len;
}

