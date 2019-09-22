/*  
COMP6320 Lab1.2 - server12.c
Authors: Christopher Tillery & John Watts
Date: September 26, 2019
Protocol Agreement: Lab12-RFC.pdf
Description:
	Receives message M from the client and extracts a (operand 1), b (operand 2), and c (opcode).
    Performs the operation requested and sends back the result with repeating the operands and operation.
	The size of the message M (at the application layer) should not exceed 16 bytes.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define REQUEST_LENGTH 9
#define RESPONSE_LENGTH 14

/* Global Variables */
char requestMessage[9], responseMessage[14], operationCode, isAnswerValid;
unsigned int operandA, operandB, answer;

void updatePacket() {
	memcpy(responseMessage + sizeof(char) + sizeof(unsigned int) + sizeof(unsigned int), &answer, sizeof(unsigned int));
	memcpy(responseMessage + sizeof(char) + sizeof(unsigned int) + sizeof(unsigned int) + sizeof(unsigned int), &isAnswerValid, sizeof(char));
}

void deconstructPacket() {
	memcpy(&operationCode, requestMessage, sizeof(char));
	memcpy(&operandA, requestMessage + sizeof(char), sizeof(unsigned int));
	memcpy(&operandB, requestMessage + sizeof(char) + sizeof(unsigned int), sizeof(unsigned int));
}

/*error check for division by zero. Set operationCode to 0 */
void calculateAnswer() {
	if (operandB == 0 && operationCode == '/') {
		isAnswerValid = '2';
		answer = 0;
	}
	else {
		isAnswerValid = '1';
		switch(operationCode) {
		case '+':
			answer = operandA  + operandB;
			break;
		case '-':
			answer = operandA - operandB;
			break;
		case 'x':
			answer = operandA * operandB;
			break;
		case '/':
			answer = operandA / operandB;
			break;
		}
	}
}

int main(int argc, char *argv[]) {
	int listenSocket, dataSocket, port, n;
	struct sockaddr_in serverSocketAddress, clientSocketAddress;
	socklen_t clientLength;
	pid_t pid;

	/* Argument check */
	if (argc < 2) {
		perror("Usage: ./server12 'Port Number'");
		printf("%c", '\n');
		exit(1);
	}

	/* Create the socket */
	if ((listenSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Problem creating the socket");
		printf("%c", '\n');
		exit(2);
	}
	memset(&serverSocketAddress, 0, sizeof(serverSocketAddress));
	memset(&clientSocketAddress, 0, sizeof(clientSocketAddress)); 
	port = atoi(argv[1]);
	serverSocketAddress.sin_family = AF_INET;
	serverSocketAddress.sin_addr.s_addr = INADDR_ANY;
	serverSocketAddress.sin_port = htons(port);

	/* Bind the listenSocket to the server & begin listening; set maximum number of connected hosts */
	if (bind(listenSocket, (struct sockaddr *) &serverSocketAddress , sizeof(serverSocketAddress)) < 0) {
		perror("Problem binding the socket");
		printf("%c", '\n');
		exit(3);
	}
	listen(listenSocket, 5);
	
	printf("%s\n", "Server running... waiting for connections.");

	for ( ; ; ) {
		
		/* Accept an incoming client connection */
		clientLength = sizeof(clientSocketAddress);
		dataSocket = accept(listenSocket, (struct sockaddr *) &clientSocketAddress, &clientLength);
		
		/* Concurrent connection handler */
		if ((pid = fork()) == 0) {
			close(listenSocket);
			
			/* Recieve the operation request */
			n = recv(dataSocket, requestMessage, REQUEST_LENGTH + 1, MSG_WAITALL);
			deconstructPacket();
			
			printf("\n%s", "operandA received from client: ");
			printf("%d", operandA);
			printf("\n%s", "operandB received from client: ");
			printf("%d", operandB);
			printf("\n%s", "operationCode received from client: ");
			printf("%c", operationCode);
			
			calculateAnswer();
			printf("\n%s", "The answer to the requested operation: ");
			printf("%d\n\n", answer);
			
			/* Add the answer & isAnswerValid values to the responseMessage */
			updatePacket();
			
			/* Send the responseMessage to the client */
			if (n = send(dataSocket, responseMessage, RESPONSE_LENGTH + 2, 0) < 0) {
				perror("Problem adding the packet to the buffer");
				printf("%c", '\n');
				exit(4);
			}
			
			/* Close a connected data socket; remember - server is still listening */
			close(dataSocket);
			exit(5);
		}
		close(dataSocket);
	}
}
