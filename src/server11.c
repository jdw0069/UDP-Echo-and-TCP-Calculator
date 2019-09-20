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
#include <time.h>
#include <limits.h>

#define MAXLINE 1024
#define PACKET_SIZE 1038
#define SERV_PORT 10010

/* Global Variables */
int sequence;
char string[MAXLINE], packet[PACKET_SIZE];
int sequenceNumber;
short totalMessageLength;
long timestamp;

void createPacket() {
	sequenceNumber = htonl(sequence);
	memcpy(packet + sizeof(short), &sequenceNumber, sizeof(int));
}

void deconstructPacket() {
	memcpy(&totalMessageLength, packet, sizeof(short));
	totalMessageLength = ntohs(totalMessageLength);
	memcpy(&sequenceNumber, packet + sizeof(short), sizeof(int));
	sequenceNumber = ntohl(sequenceNumber);
	memcpy(&timestamp, packet + sizeof(short) + sizeof(int), sizeof(long));
	timestamp = be64toh(timestamp);
	memcpy(&string, packet + sizeof(short) + sizeof(int) + sizeof(long), MAXLINE);
}

int main (int arc, char **argv) {
	int dataSocket, n;
	socklen_t clientLength;
	struct sockaddr_in clientSocketAddress, serverSocketAddress;
	pid_t childpid;

	sequence = 0;
	
	/* Create the socket with error detection */
	if ((dataSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Problem creating the socket");
		exit(2);
	}
	
	/* Prepare for the socket address */
	memset(&serverSocketAddress, 0, sizeof(serverSocketAddress)); 
	memset(&clientSocketAddress, 0, sizeof(clientSocketAddress)); 
	serverSocketAddress.sin_family = AF_INET;
	serverSocketAddress.sin_addr.s_addr = INADDR_ANY; // Kernel chooses source IP
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
		n = recvfrom(dataSocket, packet, PACKET_SIZE, MSG_WAITALL, (struct sockaddr *) &clientSocketAddress, &clientLength);
		printf("%s", "String received from and resent to the client: ");
		
		deconstructPacket();
		puts(string);

		/* Send back to a client */
		createPacket();
		if (n = sendto(dataSocket, packet, sizeof(packet), 0, (struct sockaddr *) &clientSocketAddress, clientLength) < 0) {
			perror("Problem adding the packet to the buffer");
			exit(4);
		}
		
		if (sequence < UINT_MAX) {
			sequence++;
		}
		else {
			sequence = 0;
		}
	}
	close(dataSocket);
}