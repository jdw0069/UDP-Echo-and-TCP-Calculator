/*  
COMP6320 Lab1.2 - client12.c
Authors: Christopher Tillery & John Watts
Date: September 26, 2019
Protocol Agreement: Lab12-RFC.pdf
Description:
	Accepts via command line two 32 bit unsigned integers (a and b) and a 
	character c that can be: '+', '-', 'x', or '/'. Bundles a, b, and c into 
	an application message M. The size of the application message M should
	not exceed 10 bytes. Sends M to the server.
*/
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <limits.h>
#include <stdio.h>

#define REQUEST_LENGTH 9
#define RESPONSE_LENGTH 14

/* Global variables */
char requestMessage[REQUEST_LENGTH], responseMessage[RESPONSE_LENGTH], operationCode, isAnswerValid;
unsigned int operandA, operandB, answer;

void createPacket() {
	memcpy(requestMessage, &operationCode, sizeof(char));
	memcpy(requestMessage + sizeof(char), &operandA, sizeof(unsigned int));
	memcpy(requestMessage + sizeof(char) + sizeof(unsigned int), &operandB, sizeof(unsigned int));
}

void deconstructPacket() {
	memcpy(&operationCode, responseMessage, sizeof(char));
	memcpy(&operandA, responseMessage + sizeof(char), sizeof(unsigned int));
	memcpy(&operandB, responseMessage + sizeof(char) + sizeof(unsigned int), sizeof(unsigned int));
	memcpy(&answer, responseMessage + sizeof(char) + sizeof(unsigned int) + sizeof(unsigned int), sizeof(unsigned int));
	memcpy(&isAnswerValid, responseMessage + sizeof(char) + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned int), sizeof(char));
}

int main(int argc, char *argv[]) 
{
	int dataSocket, port, n;
	static socklen_t serverLength;
	static struct sockaddr_in serverSocketAddress;
	struct hostent *serverHost;

	/* Argument check & assignment */
	if (argc < 3) {
		printf( "Usage: ./client12 'Host Name of the Server' 'Port Number'");
		exit(1);
	}
	port = atoi(argv[2]);

	/* Create the clientSocket */
	if ((dataSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("There was a problem creating the socket.");
		exit(2);
	}

	/* Extract IP from Hostname */
	if ((serverHost = gethostbyname(argv[1])) == 0) {
		perror("Please enter a valid host name.");
		exit(3);
	}
	memset(&serverSocketAddress, 0, sizeof(serverSocketAddress));
	serverSocketAddress.sin_family = AF_INET;
	serverSocketAddress.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr*) serverHost->h_addr_list[0])));
	serverSocketAddress.sin_port = htons(port);
	serverLength = sizeof(serverSocketAddress);

	/* Establish a connection with the server */
	if (connect(dataSocket, (struct sockaddr *) &serverSocketAddress, sizeof(serverSocketAddress)) < 0) {
		perror("Connection failure");
		exit(4);
	}
	
	printf ("\n%s", "Please enter operandA: ");
	scanf("%d", &operandA);
	getchar();
	printf ("%s", "Please enter operandB: ");
	scanf("%d", &operandB);
	getchar();
	printf ("%s", "Please enter an operationCode (+, -, x, or /): ");
	scanf("%c", &operationCode);
	
	/* Switch case for use at later time maybe? Currently handles invalid operationCode entries only */
	switch(operationCode) {
		case '+':
			break;
		case '-':
			break;
		case 'x':
			break;
		case '/':
			break;
		default:
			printf ("%s\n\n", "The operationCode you entered is invalid.");
			exit(5);
	}
	
	/* Create the 9 byte packet & send the data to the server */
	createPacket();
	if (n = send(dataSocket, requestMessage, REQUEST_LENGTH + 1, 0) < 0) {
			perror("Message send error");
			exit(6);
	}
	
	/* Recieve the 14 byte packet with operation results & validity state */
	if (n = recv(dataSocket, responseMessage, RESPONSE_LENGTH + 2, MSG_WAITALL) < 0) {
			perror("Message recieve error");
			exit(7);
	}
	deconstructPacket();
	
	if (isAnswerValid == '1') {
		printf ("%s", "Result of the requested operation: ");
		printf ("%d\n\n", answer);
	}
	else {
		printf ("%s\n\n", "The requested operation is invalid.");
	}
	
	close(dataSocket);
	return 0;
}