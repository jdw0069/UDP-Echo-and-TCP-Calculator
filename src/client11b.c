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

#define MAXLINE 1024 // max text length for echo
#define SERV_PORT 10010 // required port number

int main(int argc, char **argv) {
	int dataSocket, n;
	struct hostent* serverHost;
	socklen_t serverLength;
	struct timeval sendTime, recvTime;
	long RTT;
	struct sockaddr_in serverSocketAddress;
	char sendMessage[MAXLINE], recieveMessage[MAXLINE];
	
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
	scanf("\n%[^\n]", sendMessage);
	
	/* Send the data; clock the start time to milliseconds*/
	if (n = sendto(dataSocket, sendMessage, strlen(sendMessage), 0, (struct sockaddr *) &serverSocketAddress, serverLength) < 0) {
		perror("Message send error");
		exit(4);
	}
	gettimeofday(&sendTime, NULL);

	/* Recieve the data; clock the recieve time to milliseonds; calculate RTT */
	if (n = recvfrom(dataSocket, recieveMessage, MAXLINE, 0, (struct sockaddr *) &serverSocketAddress, &serverLength) < 0) {
		perror("The server terminated prematurely.");
		exit(4);
	}
	gettimeofday(&recvTime, NULL);
	RTT = (recvTime.tv_sec - sendTime.tv_sec) * 1000L + (recvTime.tv_usec - sendTime.tv_usec) / 1000;
	
	/* Output results */
	printf("%s", "String recieved from the server: ");
	printf("%s\n", recieveMessage);
	printf("%s", "Round Trip Time: ");
	printf("%ld", RTT);
	printf("%s\n", " milliseconds");
	
	return(0);
}