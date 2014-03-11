#ifndef __RTSP_INTERNAL_H
#define __RTSP_INTERNAL_H

#include <event.h>
#include <event2/event-config.h>
#include <event2/event_struct.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RTSP_CONNECT_TIMEOUT    60
#define RTSP_READ_TIMEOUT       30

#define RTSP_PREFIX             "rtsp://"
#define RTSP_DEFAULTPORT        554

/* Internal use only: Functions that might be missing from <sys/queue.h> */
#ifndef TAILQ_FIRST
#define TAILQ_FIRST(head)       ((head)->tqh_first)
#endif
#ifndef TAILQ_END
#define TAILQ_END(head)         NULL
#endif
#ifndef TAILQ_NEXT
#define TAILQ_NEXT(elm, field)      ((elm)->field.tqe_next)
#endif

#ifndef TAILQ_FOREACH
#define TAILQ_FOREACH(var, head, field)                 \
    for ((var) = TAILQ_FIRST(head);                 \
         (var) != TAILQ_END(head);                  \
         (var) = TAILQ_NEXT(var, field))
#endif

#ifndef TAILQ_INSERT_BEFORE
#define TAILQ_INSERT_BEFORE(listelm, elm, field) do {           \
    (elm)->field.tqe_prev = (listelm)->field.tqe_prev;      \
    (elm)->field.tqe_next = (listelm);              \
    *(listelm)->field.tqe_prev = (elm);             \
    (listelm)->field.tqe_prev = &(elm)->field.tqe_next;     \
} while (0)
#endif



enum evrtsp_connection_error {
    EVCON_RTSP_TIMEOUT,
    EVCON_RTSP_EOF,
    EVCON_RTSP_INVALID_HEADER,
    EVCON_RTSP_BUFFER_ERROR,
    EVCON_RTSP_REQUEST_CANCEL
};

struct evrtsp_request {
    struct evkeyvalq *input_headers;
    struct evkeyvalq *output_headers;
    /* address of the remote host and the port connection came from */
    char *remote_host;
    ev_uint16_t remote_port;
    /* cache of the hostname for evhttp_request_get_host */
    char *host_cache;
    char magor;
    char minor;
    int response_code;
};

#ifdef __cplusplus
}
#endif
#endif
