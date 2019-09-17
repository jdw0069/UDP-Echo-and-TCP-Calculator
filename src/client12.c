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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

int main(int argc, char *argv[]) 
{

int sockfd, portnumber, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char buffer[255];
int num1, num2, choice, ans;

if (argc < 3) {
	printf( "Usage %s hostname port\n", argv[0]);
	exit(0);
}

portnumber = atoi(argv[2]);
sockfd = socket(AF_INET, SOCK_STREAM, 0);

if (sockfd < 0) 
	printf("%s", "Error, can't open socket");



server = gethostbyname(argv[1]);
if(server == NULL) 
	printf("%s", "Error, no host found");



bzero((char *) &serv_addr, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
bcopy((char *) server ->h_addr , (char *) &serv_addr.sin_addr.s_addr, server ->h_length);
serv_addr.sin_port = htons(portnumber);

if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
	printf("%s", "Connection failed");


bzero(buffer, 255); //clear buffer

n = read(sockfd, buffer, 255);
if (n < 0) 
	printf("%s", "Error, can't read socket");


printf("Server - %s\n", buffer);
scanf("%d", &num1);
write(sockfd, &num1, sizeof(int));

bzero(buffer, 255);
n = read(sockfd, buffer, 255);
if (n < 0) 
	printf("%s", "Error, can't read socket");


printf("Server - %s\n", buffer);
scanf("%d", &num2);
write(sockfd, &num2, sizeof(int));

bzero(buffer,255);
n = read(sockfd, buffer, 255);
if (n < 0) 
	printf("%s", "Error, can't read socket");


printf("Server - %s\n", buffer);
scanf("%d", &choice);
write(sockfd, &choice, sizeof(int));


read(sockfd, &ans, sizeof(int));
printf("Server - The answer is: %d\n", ans);

close(sockfd);
return 0;

}

