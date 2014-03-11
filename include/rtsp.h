#ifndef __EVRTSP_H
#define __EVRTSP_H

#define EVRTSP_CODE_OK          200
#define EVRTSP_CODE_UNAUTHORIZE 401

enum evrtsp_cmd_type {
    EVRTSP_REQ_OPTIONS      = 1 << 0,
    EVRTSP_REQ_DESCRIBE     = 1 << 1,
    EVRTSP_REQ_ANNOUNCE     = 1 << 2,
    EVRTSP_REQ_SETUP        = 1 << 3,
    EVRTSP_REQ_PLAY         = 1 << 4,
    EVRTSP_REQ_PAUSE        = 1 << 5,
    EVRTSP_REQ_TEARDOWN     = 1 << 6,
    EVRTSP_REQ_GET_PARAMETER= 1 << 7,
    EVRTSP_REQ_SET_PARAMETER= 1 << 8,
    EVRTSP_REQ_REDIRECT     = 1 << 9,
    EVRTSP_REQ_RECORD       = 1 << 10
};


enum evrtsp_request_kind { EVRTSP_REQUEST, EVRTSP_RESPONSE };
#endif
