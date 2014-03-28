EXEC=eagle.exe
OBJS=engine.o
OBJS+=utils.o

OBJS+= libs/simclist.o
OBJS+= libs/threadpool.o
OBJS+= libs/parson.o

CFLAGS+= -DVERSION="\"0.1\""
CFLAGS+= -g -fPIC -Wall
CFLAGS+=-I./include -I/usr/local/include
CFLAGS+=-I.
CFLAGS+= -DHAVE_OPENSSL

LDFLAGS+=-lpthread
LDFLAGS+=-luv

.PHONY: eagle

all: $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o $(EXEC)
	rm -rf lib/*.a libs/*.so libs/*.o
