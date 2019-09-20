/*  
COMP6320 Lab1.1 - client11b.c
Authors: Christopher Tillery & John Watts
Date: September 26, 2019
Protocol Agreement: Lab11-RFC.pdf
Description:
	Accepts as command line the name of the echo server.
	Prompts the user to enter a string A.
	Sends the string A to Server S.
	Listens for a response from Server S.
	Prints out the response received from Server S.
	Prints out the round trip time (time between the transmission and the reception).
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

#define MAXLINE 1024
#define SERV_PORT 10010
#define PACKET_SIZE 1038

/* Global Variables */
char string[MAXLINE], packet[PACKET_SIZE];
int sequenceNumber;
short totalMessageLength;
long timestamp;

void createPacket() {
	sequenceNumber = htonl(0);
	totalMessageLength = htons(14 + strlen(string));
	memcpy(packet, &totalMessageLength, sizeof(short));
	memcpy(packet + sizeof(short), &sequenceNumber, sizeof(int));
	memcpy(packet + sizeof(short) + sizeof(int), &timestamp, sizeof(long));
	memcpy(packet + sizeof(short) + sizeof(int) + sizeof(long), &string, strlen(string));
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

int main(int argc, char **argv) {
	int dataSocket, n;
	struct hostent* serverHost;
	socklen_t serverLength;
	struct timeval sendTime, recvTime;
	long recvTimeMS, RTT;
	struct sockaddr_in serverSocketAddress;
	
	/* Argument check */
	if (argc != 2) {
		perror("Usage: ./clientName 'Host name of the server'");
		exit(1);
	}

	/* Extract IP from Hostname */
	if ((serverHost = gethostbyname(argv[1])) == 0) {
		perror("Please enter a valid host name.");
		exit(2);
	}
	
	/* Create the clientSocket */
	if ((dataSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("There was a problem creating the socket.");
		exit(3);
	}
	memset(&serverSocketAddress, 0, sizeof(serverSocketAddress));
	serverSocketAddress.sin_family = AF_INET;
	serverSocketAddress.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr*) serverHost->h_addr_list[0]))); // Convert number_dot string to binary
	serverSocketAddress.sin_port = htons(SERV_PORT); // Host byte order -> Network byte order
	serverLength = sizeof(serverSocketAddress);
	
	printf ("%s", "Please enter a string to send to the server: ");
	scanf("\n%[^\n]", string);
	
	/* Get current timestamp and format for transport */
	gettimeofday(&sendTime, NULL);
	timestamp = htobe64((sendTime.tv_sec) * 1000L + (sendTime.tv_usec) / 1000);
	createPacket();
	
	/* Send the data; clock the start time to milliseconds*/
	if (n = sendto(dataSocket, packet, PACKET_SIZE, 0, (struct sockaddr *) &serverSocketAddress, serverLength) < 0) {
		perror("Message send error");
		exit(4);
	}
	
	/* Recieve the data; clock the recieve time to milliseonds; calculate RTT */
	if (n = recvfrom(dataSocket, packet, PACKET_SIZE, 0, (struct sockaddr *) &serverSocketAddress, &serverLength) < 0) {
		perror("The server terminated prematurely.");
		exit(4);
	}
	
	/* Get current timestamp and get timestamp from the recv packet; calculate RTT */
	gettimeofday(&recvTime, NULL);
	recvTimeMS = (recvTime.tv_sec) * 1000L + (recvTime.tv_usec) / 1000;
	deconstructPacket();
	RTT = recvTimeMS - timestamp;
	
	/* Output results */
	printf("%s", "String recieved from the server: ");
	printf("%s\n", string);
	printf("%s", "Round Trip Time: ");
	printf("%ld", RTT);
	printf("%s\n", " millisecond(s)");
	
	return(0);
}