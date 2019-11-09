#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
struct client_req {
	int a;
	int b;
};

struct client_req * request;
char data_buff[100];
int result;
static void service_client(int client_fd, struct sockaddr_in *client) {
	int addr_len = sizeof(struct sockaddr_in);
	int sent_recv_bytes = 0;

	while(1) {
		sent_recv_bytes = recvfrom(client_fd , (char *)data_buff, sizeof(data_buff),0,(struct sockaddr *)client, &addr_len);
		if(sent_recv_bytes <= 0)
		{
			close(client_fd);
			free(client);
			printf("\nNo bytes recieved from Client ,hence closing the client connection");
			break;
		}
		request = (struct client_req *)data_buff;			
		if (request->a == 0 && request->b ==0)
		{
			close(client_fd);
			free(client);
			printf("\n Client Requested to disconnect the connection");
			break;
		}
		result = request->a + request->b;

		sent_recv_bytes = sendto(client_fd, (char *)&result, sizeof(int), 0,(struct sockaddr *)client, sizeof(struct sockaddr) );
		
		printf("\nSent %d bytes in reply to client",sent_recv_bytes);

	}
	         
}


void setup_tcp_serv_comm()
{
	int master_fd =0, client_fd =0, addr_len = 0;
	struct sockaddr_in serv_addr,client_addr;
	fd_set readfds;
	if((master_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		printf("\nsocket creation failed");
		exit(1);
	}
	memset(&serv_addr,0x00,sizeof(struct sockaddr_in));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = 2000;
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	addr_len = sizeof(struct sockaddr);

	if(bind(master_fd,(struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1){
		printf("\n socket bind error");
		return;
	}

	if(listen(master_fd,5) < 0)
	{
		printf("\nlisten failed");
		return;
	}

	while(1){

		// initialize and fill readfds
		FD_ZERO(&readfds);	/* Initialize the file descriptor set */
		FD_SET(master_fd, &readfds); /* Add the socket to this on which */

		printf("\n blocked on select system call ...");

		// Wait for client connection
		select(master_fd+1, &readfds, NULL, NULL, NULL);  /*Call the select system cal, server process blocks here. Linux OS keeps this process blocked untill the data arrives on any of the file Descriptors in the 'readfds' set*/
		/*New connection request arrived */
		if(FD_ISSET(master_fd,&readfds)){

			printf("\nNew connection arrived");
			client_fd = accept(master_fd, (struct sockaddr *)&client_addr,&addr_len);

			if( client_fd < 0 ) {
				printf("\n Accept Error :%d\n", errno);
				exit(0);
			}
			printf("\nConnection accepted from client : %s:%u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

			if(fork() == 0) {
				struct sockaddr_in *clientaddr = calloc(1, sizeof(struct sockaddr_in));
				memcpy(clientaddr, &client_addr, sizeof(struct sockaddr_in));
				printf("\n Child process Created");
				service_client(client_fd,clientaddr);
				printf("\n Child process terminates");
				exit(0);
			}

		}
		

		

	}



}

int main(int argc, char *argv)
{
	setup_tcp_serv_comm();
	
	return 0;
}

