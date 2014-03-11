EXEC=eagle.exe

OBJS= eagle.o
OBJS+= utils.o
OBJS+= workqueue.o

OBJS+= libs/simclist.o
OBJS+= libs/threadpool.o
OBJS+= libs/parson.o
CFLAGS+= -DVERSION="\"0.1\""
CFLAGS+= -g -fPIC -Wall -I./include -I/usr/local/include
CFLAGS+= -DHAVE_OPENSSL

LDFLAGS+=-lpthread
LDFLAGS+=-levent

.PHONY: eagle

all: $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o lib/*.a libs/*.so $(EXEC)
