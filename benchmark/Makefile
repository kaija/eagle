CFLAGS+= -I../include
all: tcp

tcp: tcp.o ../utils.o ../libs/parson.o
	$(CC) -o tcp.exe tcp.o ../utils.o ../libs/parson.o $(CFLAGS)
clean:
	rm -rf *.o tcp.exe
