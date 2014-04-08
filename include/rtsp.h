#ifndef __EVRTSP_H
#define __EVRTSP_H

#define EVRTSP_CODE_OK          200
#define EVRTSP_CODE_UNAUTHORIZE 401

#define RTSP_VER                "1.0"
#define RTSP_USER_AGENT         "Eagle Agent"

enum rtsp_cmd_type {
    RTSP_REQ_OPTIONS      = 1 << 0,
    RTSP_REQ_DESCRIBE     = 1 << 1,
    RTSP_REQ_ANNOUNCE     = 1 << 2,
    RTSP_REQ_SETUP        = 1 << 3,
    RTSP_REQ_PLAY         = 1 << 4,
    RTSP_REQ_PAUSE        = 1 << 5,
    RTSP_REQ_TEARDOWN     = 1 << 6,
    RTSP_REQ_GET_PARAMETER= 1 << 7,
    RTSP_REQ_SET_PARAMETER= 1 << 8,
    RTSP_REQ_REDIRECT     = 1 << 9,
    RTSP_REQ_RECORD       = 1 << 10
};

enum rtsp_resp_type {
    RTSP_RESP_OPTIONS      = 1 << 0,
    RTSP_RESP_DESCRIBE     = 1 << 1,
    RTSP_RESP_ANNOUNCE     = 1 << 2,
    RTSP_RESP_SETUP        = 1 << 3,
    RTSP_RESP_PLAY         = 1 << 4,
    RTSP_RESP_PAUSE        = 1 << 5,
    RTSP_RESP_TEARDOWN     = 1 << 6,
    RTSP_RESP_GET_PARAMETER= 1 << 7,
    RTSP_RESP_SET_PARAMETER= 1 << 8,
    RTSP_RESP_REDIRECT     = 1 << 9,
    RTSP_RESP_RECORD       = 1 << 10
};

struct rtsp_client{
    int                 state;
    int                 codec;
    int                 fps;
    int                 gop_size;
};

void rtsp_get_time();

#endif
