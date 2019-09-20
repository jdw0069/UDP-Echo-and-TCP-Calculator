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

int main(int argc, char *argv[]) {


//check for port number from client

if (argc < 2) {
	printf("%s", "No port number provided\n");
	exit(1);
}

int sockfd, newsockfd, portnumber, n;
char buffer[255];
int num1, num2, ans, choice;
struct sockaddr_in serv_addr, cli_addr;
socklen_t clilen;

sockfd = socket(AF_INET, SOCK_STREAM, 0);

if (sockfd < 0) {
	printf("%s", "Could not open socket.");
}
//clears any data that it is referenced to
bzero((char *) &serv_addr, sizeof(serv_addr));

portnumber = atoi(argv[1]);

serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = INADDR_ANY;
serv_addr.sin_port = htons(portnumber); //conversion

//now bind
if(bind(sockfd, (struct sockaddr *) &serv_addr , sizeof(serv_addr)) < 0) {
	printf("%s", "Could not bind socket");
}

listen(sockfd, 5); //5 is max limit that can connect at one time
clilen = sizeof(cli_addr);

//accept

newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

if (newsockfd < 0) {
	printf("%s", "Error could not accept");
}


n = write(newsockfd, "Enter fist number: ", strlen("Enter first number"));

if (n < 0) {
	printf("%s", "Error, can't write to socket");
}

read(newsockfd, &num1, sizeof(int));
printf("Client - Number 1 is %d\n", num1);


n = write(newsockfd, "Enter second number: ", strlen("Enter second number"));

if (n < 0) {
	printf("%s", "Error, can't write to socket");
}

read(newsockfd, &num2, sizeof(int));
printf("Client - Number 2 is  %d\n", num2);;


n = write(newsockfd, "Enter an operand: \n1.Addition(+)\n2.Subtraction(-)\n3.Multiplication(*)\n4.Division(/)\n", strlen("Enter an operand: \n1.Addition(+)\n2.Subtraction\n3.Multiplication\n4.Division(/)\n"));

if (n < 0) {
	printf("%s", "Error, can't write to socket");
}

read(newsockfd, &choice, sizeof(int));
printf("Client - Operand is: %d\n", choice);

switch(choice) {

	case 1:
		ans = num1  + num2;
		break;
	
	case 2:
		ans = num1 - num2;
		break;

	case 3:
		ans = num1 * num2;
		break;

	case 4:
		ans = num1/num2;
		break;
}


write(newsockfd, &ans, sizeof(int));



close(newsockfd);
close(sockfd);
return 0;

}
