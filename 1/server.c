#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>

#define NUM 6

int create_a_socket(int *sock) {
	struct sockaddr_in server;

	if ((*sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		return 1; 
	}

	server.sin_family = AF_INET;
	server.sin_port = 0;
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(*sock, (struct sockaddr *) &server, sizeof(server)) == -1) {
		perror("cannot bind socket"); 
		return 1;
	}

	socklen_t len = sizeof(server);
	getsockname(*sock, &server, &len);

	printf("Server port: %d\n", ntohs(server.sin_port));

	return 0;
}

int main(int argc, char *argv[]) {
	int sock;
	//int client_sock;
	int end_buffer;
	int client_len;
	char buffer[1024];
	char char_to_send;
	struct sockaddr_in client;

	if (create_a_socket(&sock) == 1) 
		return 0;

	client_len = sizeof(client);
	char_to_send = 'A';

	while (1) {
		bzero(&buffer, sizeof(buffer));
		end_buffer = recvfrom(sock, &buffer, sizeof(buffer), 0, ( struct sockaddr *) &client, &client_len);

		printf("=========================================\n");
		printf("Client address: %s\n", inet_ntoa(client.sin_addr));
		printf("Client port: %d\n", client.sin_port);
		buffer[end_buffer] = '\0';
		printf("Client msg: %s\n", buffer);
		buffer[0] = char_to_send;
		printf("Send msg: %s\n", buffer);
		printf("=========================================\n");

		sendto(sock, buffer, strlen(buffer), 0, (const struct sockaddr *) &client, client_len);
	}

	close(sock);

	return 0;
}
