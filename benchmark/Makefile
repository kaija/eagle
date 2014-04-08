CFLAGS+= -I../include

tcp_obj=tcp.o ../client/utils.o ../libs/parson.o 

all: tcp

tcp: $(tcp_obj)
	$(CC) -o tcp.exe $(tcp_obj) $(CFLAGS)
clean:
	rm -rf *.o tcp.exe
