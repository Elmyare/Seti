#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/wait.h>

#define NUM 5
#define CLIENTS 1

int create_a_socket(int *sock) {
	struct sockaddr_in server;

	if ( (*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) { 
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

void client_handle(int sock, const char * ip_address, int port) {
	int end_buffer;
	char char_to_send;
	char buffer[1024];

	char_to_send = 'A';

	for (int i = 0; i < NUM; i++) {
		bzero(&buffer, sizeof(buffer));
		end_buffer = recv(sock, &buffer, sizeof(buffer), 0);	

		printf("=========================================\n");
		printf("Client address: %s\n", ip_address);
		printf("Client port: %d\n", port);
		buffer[end_buffer] = '\0';
		printf("Client: %s\n", buffer);
		buffer[0] = char_to_send;
		printf("Changed to: %s\n", buffer);
		printf("=========================================\n");

		send(sock, &buffer, sizeof(buffer), 0);
	}
}

void wait_process(int signum) {
    wait(NULL);
}

int main(int argc, char *argv[]) {
	int sock;
	int client_sock;
	int client_len;
	struct sockaddr_in client;
	pid_t pid;

	if (create_a_socket(&sock) == 1) 
		return 1;

    if ((listen(sock, CLIENTS)) != 0) { 
        perror("listen failed"); 
        return 1; 
    } 

	client_len = sizeof(client);

	signal(SIGCHLD, wait_process);

	while (1) {
		client_sock = accept(sock, (struct sockaddr*)&client, &client_len);

		pid = fork();

		if (pid == 0) {
			close(sock);
			client_handle(client_sock, inet_ntoa(client.sin_addr), client.sin_port);
			close(client_sock);
			exit(0);
		} else {
			close(client_sock);
//			if (waitpid(pid, NULL, 0) < 0) {
//                perror("failed to collect child process");
//                break;
//            }
		}
	}

	close(sock);

	return 0;
}
