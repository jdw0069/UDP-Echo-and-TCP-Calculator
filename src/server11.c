/* 
COMP6320 Lab1.1 - server11.c
Authors: Christopher Tillery & John Watts
Date: September 26, 2019
Protocol Agreement: Lab11-RFC.pdf
Description:
	Concurrent datagram (UDP) echo server S
	This server must send back any message you send to it.
	The server must "listen" on port 10010. 
	The server must be able to handle concurrently multiple requests.
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <string.h>
#include <unistd.h>

#define MAXLINE 1024 // max text length for echo
#define SERV_PORT 10010 // required port number

int main (int arc, char **argv) {
	int dataSocket, n;
	socklen_t clientLength;
	char buffer[MAXLINE]; // for send and recv data
	struct sockaddr_in clientSocketAddress, serverSocketAddress;
	pid_t childpid;
	
	/* Create the socket with error detection */
	if ((dataSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Problem creating the socket");
		exit(2);
	}
	
	/* Prepare for the socket address */
	memset(&serverSocketAddress, 0, sizeof(serverSocketAddress)); 
	serverSocketAddress.sin_family = AF_INET;
	serverSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Kernel chooses source IP
	serverSocketAddress.sin_port = htons(SERV_PORT); // Host byte order -> Network byte order
	clientLength = sizeof(clientSocketAddress);

	if (bind(dataSocket, (struct sockaddr *) &serverSocketAddress, sizeof(serverSocketAddress)) < 0) {
		perror("Problem binding the socket");
		exit(3);
	}
	
	printf("%s\n", "Server running... waiting for connections.");
	
	/* Client handler */
	for ( ; ; ) {
		
		/* Recieve from a client */
		n = recvfrom(dataSocket, buffer, MAXLINE, 0, (struct sockaddr *) &clientSocketAddress, &clientLength);
		printf("%s", "String received from and resent to the client: ");
		buffer[n] = 0; // Ending flag for the string
		puts(buffer);
		
		/* Send back to a client */
		n = sendto(dataSocket, buffer, n, 0, (struct sockaddr *) &clientSocketAddress, clientLength);
		buffer[0] = 0;
	}
	
	close(dataSocket);
}