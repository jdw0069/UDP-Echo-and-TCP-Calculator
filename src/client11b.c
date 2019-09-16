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
#include <arpa/inet.h>
#include <string.h>

#define MAXLINE 1024 // max text length for echo
#define SERV_PORT 10010 // required port number

int main(int argc, char **argv) {
	int dataSocket, n;
	socklen_t serverLength;
	struct sockaddr_in serverSocketAddress;
	char sendMessage[MAXLINE], recieveMessage[MAXLINE];
	
	/* Argument check */
	if (argc != 2) {
		perror("Usage: TCPClient <IP address of the server");
		exit(1);
	}
	
	/* Create the clientSocket */
	if ((dataSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("There was a problem creating the socket.");
		exit(2);
	}
	memset(&serverSocketAddress, 0, sizeof(serverSocketAddress));
	serverSocketAddress.sin_family = AF_INET;
	serverSocketAddress.sin_addr.s_addr = inet_addr(argv[1]); // Convert number_dot string to binary
	serverSocketAddress.sin_port = htons(SERV_PORT); // Host byte order -> Network byte order
	serverLength = sizeof(serverSocketAddress);
	
	printf ("%s", "Please enter a string to send to the server: ");
	scanf("\n%[^\n]", sendMessage);
	
	/* Send the data */
	if (n = sendto(dataSocket, sendMessage, strlen(sendMessage), 0, (struct sockaddr *) &serverSocketAddress, serverLength) < 0) {
		perror("Message send error");
		exit(3);
	}

	if (n = recvfrom(dataSocket, recieveMessage, MAXLINE, 0, (struct sockaddr *) &serverSocketAddress, &serverLength) < 0) {
		perror("The server terminated prematurely.");
		exit(4);
	}
	
	printf("%s", "String recieved from the server: ");
	printf("%s\n", recieveMessage);	
	
	return(0);
}