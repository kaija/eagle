#ifndef __EAGLE_H
#define __EAGLE_H
#include <event.h>

#ifndef VERSION
#define VERSION "0.1"
#endif
/* Port to listen on. */
#define SERVER_PORT 8554
/* Connection backlog (# of backlogged connections to accept). */
#define CONNECTION_BACKLOG 8
/* Socket read and write timeouts, in seconds. */
#define SOCKET_READ_TIMEOUT_SECONDS 10
#define SOCKET_WRITE_TIMEOUT_SECONDS 10
/* Number of worker threads.  Should match number of CPU cores reported in /proc/cpuinfo. */
#define NUM_THREADS 2

/* Behaves similarly to fprintf(stderr, ...), but adds file, line, and function
 information. */
#define errorOut(...) {\
    fprintf(stderr, "%s:%d: %s():\t", __FILE__, __LINE__, __FUNCTION__);\
    fprintf(stderr, __VA_ARGS__);\
}

enum CLIENT_ROLE {
    ROLE_PLAYER,
    ROLE_IPCAM
};

/**
 * Struct to carry around connection (client)-specific data.
 */
typedef struct client {
    /* The client's socket. */
    int fd;
    /* The event_base for this client. */
    struct event_base *evbase;
    /* The bufferedevent for this client. */
    struct bufferevent *buf_ev;
    /* The output buffer for this client. */
    struct evbuffer *output_buffer;
    /* Here you can add your own application-specific attributes which
     * are connection-specific. */

} client_t;

#endif
