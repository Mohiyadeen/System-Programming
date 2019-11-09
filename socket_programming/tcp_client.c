#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 2000

struct client_req {
	int a;
	int b;
}request;

char buf[100];

void setup_tcp_client() {
	int client_fd;
	struct sockaddr_in server_addr;
	int sum;
	int sent_recv_bytes;
	int addr_len = sizeof(struct sockaddr_in);
	client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(client_fd < 0){
		printf("\n Client socket failed");
		return;
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = PORT;
	if(connect(client_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1){
		printf("\n Connection to server failed :%d", errno);
		return;
	}
	while(1) {
		printf("\n Please Enter the numbers to be added :"); 
		scanf("%d %d", &request.a, &request.b);
		
		sent_recv_bytes = sendto(client_fd,(char *)&request, sizeof(struct client_req), 0,
					(struct sockaddr *) &server_addr, sizeof(struct sockaddr) );
		if( request.a == 0 && request.b == 0)
		{
			close(client_fd);
			printf("\nConnection Closed Exiting...\n");
			break;
		}
		sent_recv_bytes = recvfrom(client_fd,(char *)buf, sizeof(buf), 0,
                                        (struct sockaddr *) &server_addr, &addr_len);
		
		sum = *(int *)buf;
		
		printf("\nResult from server is %d", sum);

	}

}

void main(int argc, char *argv[])
{
	setup_tcp_client();
	return;
}
