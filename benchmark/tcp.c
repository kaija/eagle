#include <stdio.h>
#include <errno.h>
#include "parson.h"
#include "utils.h"
int main()
{
	int sk = noly_tcp_connect("127.0.0.1", 8554);
	if(sk > 0) {
		printf("Connect to server successfully %d\n", sk);
	}else{
		printf("Connect ot server failure code %d\n", errno);
		return -1;
	}
	while(1){
		char buf[1400];
		//usleep(10);
		send(sk, buf, 1400, 0);
	}
	return 0;
}
