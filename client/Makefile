EXEC=eagle_cli.exe

OBJS=log.o
OBJS+=utils.o
OBJS+=client.o

#OBJS+= libs/simclist.o
#OBJS+= libs/threadpool.o
OBJS+= parson.o

#CFLAGS+= -DVERSION="\"0.1\""
CFLAGS+= -g -fPIC -Wall
CFLAGS+= -Werror
CFLAGS+=-I./include -I/usr/local/include
CFLAGS+=-I.
CFLAGS+= -DHAVE_OPENSSL

LDFLAGS+=-lcrypto
#LDFLAGS+=-luv

.PHONY: all

all: $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o $(EXEC)
	rm -rf lib/*.a libs/*.so libs/*.o
