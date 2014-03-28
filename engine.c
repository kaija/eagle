#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include <utils.h>

static const int PORT = 8080;
static const int DEBUG = 1;

typedef struct {
  uv_write_t req;
  uv_buf_t buf;
} write_req_t;

static uv_loop_t *loop;
static uv_loop_t *client_loop;
static uv_handle_t* server;
static uv_tcp_t tcpServer; // tcp network io
static int server_closed;
uv_tcp_t * client_handle;

static void on_connection(uv_stream_t*, int status);
static void after_write(uv_write_t* req, int status);
static void after_read(uv_stream_t*, ssize_t nread, const uv_buf_t* buf);

static void alloc_buffer(uv_handle_t* handle,
                       size_t suggested_size,
                       uv_buf_t* buf) {
    buf->base = malloc(suggested_size);
    buf->len = suggested_size;
}

static int read_somaxconn()
{
    int conn = SOMAXCONN;   //use system default value
    char buf[64] = "";
    if(read_proc("/proc/sys/net/core/somaxconn", buf, 64) == 0) {
        conn = atoi(buf);
    }
    return conn;
}

static void on_client_connection(uv_stream_t* server, int status) {
    if (DEBUG) {
        fprintf(stdout, "Debug: on_connection: %d\n", status);
    }

    if (status == -1) {
        return;
    }

    client_handle = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    
    uv_tcp_init(loop, client_handle);


    if (uv_accept(server, (uv_stream_t*) client_handle) == 0) {
        uv_read_start((uv_stream_t*) client_handle, alloc_buffer, after_read);
    }
    else {
        uv_close((uv_handle_t*) client_handle, NULL);
    }
}

static void on_connection(uv_stream_t* server, int status) {
    if (DEBUG) {
        fprintf(stdout, "Debug: on_connection: %d\n", status);
    }
  
    if (status == -1) {
        return;
    }
    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);

    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        uv_read_start((uv_stream_t*) client, alloc_buffer, after_read);
    }
    else {
        uv_close((uv_handle_t*) client, NULL);
    }
}

static void on_close(uv_handle_t* peer) {
    if (DEBUG) {
        fprintf(stdout, "Debug: on_close clear: %d\n", 0);
    }
    free(peer);
}

static void after_shutdown(uv_shutdown_t* req, int status) {
    if (DEBUG) {
        fprintf(stdout, "Debug: after_shutdown: %d\n", status);
    }

    uv_close((uv_handle_t*)req->handle, on_close);
    free(req);

}

static void on_server_close(uv_handle_t* handle) {
    if (DEBUG) {
        fprintf(stdout, "Debug: on_server_close: %d\n", 0);
    }
}

static void after_write(uv_write_t* req, int status) {
/*
    write_req_t* wr;

    wr = (write_req_t*) req;
    free(wr->buf.base);
    free(wr);

    if (DEBUG) {
        fprintf(stdout, "Debug: after_write: %d\n", status);
        printf("Debug: after_write: close connection\n");
    }
*/
}


static void forward (ssize_t nread, const uv_buf_t* buf) {
    write_req_t *wr;
    wr = (write_req_t*) malloc(sizeof *wr);
    wr->buf = uv_buf_init(buf->base, nread);

    if (DEBUG) {
        fprintf(stdout, "Debug: write buf: %s\n", wr->buf.base);
    }
    if (client_handle) {
        if (uv_write(&wr->req, (uv_stream_t *)client_handle, &wr->buf, 1, after_write)) {
            fprintf(stderr, "uv_write failed %s\n", "");
        }
    }
    free(buf->base);
}

static void after_read(uv_stream_t* handle,
                       ssize_t nread,
                       const uv_buf_t* buf) {
    //int i;
    //write_req_t *wr;
    uv_shutdown_t* req;
  
    if (DEBUG) {
        fprintf(stdout, "Debug: after_read: %zd\n", nread);
        fprintf(stdout, "Debug: after_read: %s\n", buf->base);
    }
  
    if (nread < 0) {
      /* Error or EOF */
  
      if (buf->base) {
        free(buf->base);
      }
  
      req = (uv_shutdown_t*) malloc(sizeof *req);
      uv_shutdown(req, handle, after_shutdown);
  
      return;
    }
    forward (nread, buf);
}

int main() {
    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", PORT, &addr);
    loop = uv_default_loop();
    int max_conn = read_somaxconn();
    printf("=========================================================================\n\n");
    printf("    Bootstrap Environment\n\n");
    fprintf(stdout, "    PORT: %d \n", PORT);
    fprintf(stdout, "    SOMAXCONN: %d \n\n", max_conn);
    printf("=========================================================================\n\n");

    uv_tcp_t server;
    uv_tcp_init(loop, &server);
    uv_tcp_bind(&server, (const struct sockaddr*) &addr, 0);
    int r = uv_listen((uv_stream_t*) &server, max_conn, on_connection);

    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_err_name(r));
        return 1;
    }

    struct sockaddr_in c_addr;
    uv_ip4_addr("0.0.0.0", 443, &c_addr);
    printf("=========================================================================\n\n");
    fprintf(stdout, "    PORT: %d \n", PORT);
    fprintf(stdout, "    SOMAXCONN: %d \n\n", max_conn);
    printf("=========================================================================\n\n");

    uv_tcp_t client_server;
    uv_tcp_init(loop, &client_server);
    uv_tcp_bind(&client_server, (const struct sockaddr*) &c_addr, 0);
    r = uv_listen((uv_stream_t*) &client_server, max_conn, on_client_connection);

    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_err_name(r));
        return 1;
    }

    return uv_run(loop, UV_RUN_DEFAULT);
}
