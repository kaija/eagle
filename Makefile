EXEC=eagle.exe
OBJS=engine.o
OBJS+=utils.o
OBJS+=rtsp.o

OBJS+= libs/simclist.o
OBJS+= libs/threadpool.o
OBJS+= libs/parson.o

CFLAGS+= -DVERSION="\"0.1\""
CFLAGS+= -g -fPIC -Wall
CFLAGS+= -Werror
CFLAGS+=-I./include -I/usr/local/include
CFLAGS+=-I.
CFLAGS+= -DHAVE_OPENSSL

LDFLAGS+=-lpthread
LDFLAGS+=-luv

.PHONY: eagle client benchmark

all: eagle client

eagle: $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(CFLAGS) $(LDFLAGS)

client:
	$(MAKE) -C client

benchmark:
	$(MAKE) -C benchmark

clean:
	rm -rf *.o $(EXEC)
	rm -rf lib/*.a libs/*.so libs/*.o
	$(MAKE) -C benchmark clean
	$(MAKE) -C client clean
