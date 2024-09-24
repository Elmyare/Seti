#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define NUM 100

int main(void) {
	int sock;
	int port;
	int end_buffer;
	int r;
	char buffer[1024];
	char server_address[1024];
	pthread_t send_handler;
	pthread_t recieve_handler;
	struct sockaddr_in server;
	socklen_t server_len;

	printf("Input ip address: ");
	fgets(server_address, sizeof(server_address), stdin);
	printf("Input server port: ");
	scanf("%d", &port);

	server_address[strcspn(server_address, "\n")] = 0; // delete newline from msg

	if ( (sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		sock = 0; 
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	//server.sin_addr.s_addr = inet_addr("127.0.0.1");
	inet_pton(AF_INET, server_address, &(server.sin_addr));

	srand(time(NULL));
	r = (rand() % 5) + 1;

	for (int i = 0; i < NUM; i++) {
		sprintf(buffer, "%d", r);
		sendto(sock, (const char *)buffer, strlen(buffer),
			MSG_CONFIRM, (const struct sockaddr *) &server,
				sizeof(server));

		end_buffer = recvfrom(sock, (char *)buffer, sizeof(buffer),  
					MSG_WAITALL, (struct sockaddr *) &server, 
					&server_len); 
		buffer[end_buffer] = '\0';

		printf("Server msg: %s\n", buffer);

		sleep(r);
	}


	close(sock);

	return 0;
}
