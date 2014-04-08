#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <openssl/evp.h>
#include <openssl/aes.h>

#include "eagle_cli.h"
#include "utils.h"
#include "log.h"


void *cvr_rtsp_header_lookup(char *pkt, char *key, int *val_len)
{
    if(!pkt) return NULL;
    char *newline;
    char *buf = pkt;
    printf("RTSP header\n");
    int count = 0;
    for(;;){
        newline = strstr(buf, "\r\n");
        if(newline != NULL){
            //SDP here
            newline+=2;
            //printf("%02d %s\n", count, newline);
            buf = newline;
        }else{
            break;
        }
        count ++;
    }
    return NULL;
}

static int rtsp_cseq = 2;
void cvr_rtsp_cseq_incr()
{
    rtsp_cseq ++;
}

int cvr_rtsp_cseq_get()
{
    return rtsp_cseq;
}

int cvr_send_options(int fd)
{
    char pkt[RTSP_PKT_LEN] = "";
    int pkt_len = snprintf(pkt, RTSP_PKT_LEN, "OPTIONS rtsp://%s:%d/%s RTSP/1.0\r\n"
                            "CSeq: %d\r\n"
                            "User-Agent: Eagles (Kaija 0.0.1)\r\n\r\n", IPCAM_IP, IPCAM_PORT, RTSP_PLAY_PATH, cvr_rtsp_cseq_get());
    LOG(LOG_DEBUG,"Generate RTSP OPTIONS\n%s\n", pkt);
    return send(fd, pkt, pkt_len, 0);
}

int cvr_send_describe(int fd)
{
    char pkt[RTSP_PKT_LEN] = "";
    int pkt_len = snprintf(pkt, RTSP_PKT_LEN, "DESCRIBE rtsp://%s:%d/%s RTSP/1.0\r\n"
                            "CSeq: %d\r\n"
                            "User-Agent: Eagles (Kaija 0.0.1)\r\n"
                            "Accept: application/sdp\r\n\r\n", IPCAM_IP, IPCAM_PORT, RTSP_PLAY_PATH, cvr_rtsp_cseq_get());
    LOG(LOG_DEBUG,"Generate RTSP DESCRIBE\n%s\n", pkt);
    return send(fd, pkt, pkt_len, 0);
}

int cvr_send_setup(int fd)
{
    char pkt[RTSP_PKT_LEN] = "";
    int pkt_len = snprintf(pkt, RTSP_PKT_LEN, "SETUP rtsp://%s:%d/%s/track1 RTSP/1.0\r\n"
        "CSeq: %d\r\n"
        "User-Agent: Eagles (Kaija 0.0.1)\r\n"
        "Transport: RTP/AVP/TCP;unicast;interleaved=0-1\r\n\r\n", IPCAM_IP, IPCAM_PORT, RTSP_PLAY_PATH, cvr_rtsp_cseq_get());
    LOG(LOG_DEBUG,"Generate RTSP SETUP\n%s\n", pkt);
    return send(fd, pkt, pkt_len, 0);
}
/*
PLAY rtsp://192.168.1.248/channel2/ RTSP/1.0

CSeq: 5

User-Agent: LibVLC/2.0.8 (LIVE555 Streaming Media v2011.12.23)

Session: 4

Range: npt=0.000-
*/


int cvr_send_play(int fd, char *session)
{
    if(!session) return -1;
    char pkt[RTSP_PKT_LEN] = "";
    int pkt_len = snprintf(pkt, RTSP_PKT_LEN, "PLAY rtsp://%s:%d/%s/track1 RTSP/1.0\r\n"
        "CSeq: %d\r\n"
        "User-Agent: Eagles (Kaija 0.0.1)\r\n"
        "Session: %s\r\n"
        "Range: npt=0.000-\r\n\r\n", IPCAM_IP, IPCAM_PORT, RTSP_PLAY_PATH, cvr_rtsp_cseq_get(), session);
    LOG(LOG_DEBUG,"Generate RTSP PLAY\n%s\n", pkt);
    return send(fd, pkt, pkt_len, 0);
}
static int pcount = 0;
void cvr_progressing()
{
    pcount ++;
    if(pcount % 4 == 0)
    printf("\r| ");
    if(pcount % 4 == 1)
    printf("\r\\ ");
    if(pcount % 4 == 2)
    printf("\r--");
    if(pcount % 4 == 3)
    printf("\r/ ");
    fflush(stdout);
}

int rtp_pkt_handler(int fd, char *pkt, int len)
{
    char *rtp = pkt;
    char *h264 = NULL;
    if(pkt[0] == 0x24 && pkt[1] == 0x00){
        rtp += RTP_ITLV_LEN;        //skip rtsp interleaved
        h264 = rtp + RTP_HDR_LEN;   //skip rtp header
        noly_hexdump((unsigned char *)h264, 16);
    }
    return 0;
}


int main()
{
    char *rtp = NULL;
    size_t len = 0;
    int sfd = 0;
    char buf[RTSP_PKT_LEN];
    fprintf(stdout, "Eagle client start\n");

    int fd = noly_tcp_connect(IPCAM_IP, IPCAM_PORT);
    if(fd <= 0) {
        fprintf(stderr, "COnnect to local media server error\n");
        return -1;
    }
    fprintf(stdout, "Connected to %s:%d\n", IPCAM_IP, IPCAM_PORT);
#if 0
    int sfd = noly_tcp_connect(SERVER_IP, SERVER_PORT);
    if(sfd <= 0) {
        fprintf(stderr, "COnnect to Eagle Media server error\n");
        return -1;
    }
    fprintf(stdout, "Connected to %s:%d\n", SERVER_IP, SERVER_PORT);
#endif
    if(cvr_send_options(fd)>0){
        usleep(10000);
        memset(buf, 0, RTSP_PKT_LEN);
        len = read(fd, buf, RTSP_PKT_LEN);
        cvr_rtsp_cseq_incr();
        LOG(LOG_DEBUG, "\n%s\n", buf);
        cvr_send_describe(fd);
        usleep(10000);
        memset(buf, 0, RTSP_PKT_LEN);
        len = read(fd, buf, RTSP_PKT_LEN);
        cvr_rtsp_cseq_incr();
        LOG(LOG_DEBUG, "\n%s\n", buf);
        usleep(10000);
        cvr_send_setup(fd);
        cvr_rtsp_header_lookup(buf, NULL, NULL);
        usleep(100000);
        memset(buf, 0, RTSP_PKT_LEN);
        len = read(fd, buf, RTSP_PKT_LEN);
        cvr_rtsp_cseq_incr();
        cvr_send_play(fd,"4");
        usleep(100000);
        memset(buf, 0, RTSP_PKT_LEN);
        len = read(fd, buf, RTSP_PKT_LEN);
        cvr_rtsp_cseq_incr();
        LOG(LOG_DEBUG, "\n%s\n", buf);
        while(1){
            len = read(fd, buf, RTSP_PKT_LEN);
            if (len > 0 ) {
                    rtp = buf;
                    //noly_hexdump(rtp, 16);
                    rtp += RTP_ITLV_LEN;
                    rtp += RTP_HDR_LEN;
                    //noly_hexdump(rtp, 4);
                    int ret = send(sfd, buf, len, 0);
                    if(ret < 0){
                        //LOG(LOG_ERROR,"Server connection error %d  %s\n", ret , strerror(errno));
                        //break;
                    }
                cvr_progressing();
            }
        }
    }
    return 0;
}
