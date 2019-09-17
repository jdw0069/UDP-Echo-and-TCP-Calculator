/*  
COMP6320 Lab1.1 - client11c.c
Authors: Christopher Tillery & John Watts
Date: September 26, 2019
Protocol Agreement: Lab11-RFC.pdf
Description:
	The first process runs a loop which sends continuously the 
	numbers from 1 to 10,000 as strings (so 10,000 strings in total).
	The second process receives the responses from the server and reports any missing echo.
	At the statistics summary line, report whether there are missing echoes,
	and the smallest, largest, and average round trip times. 
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAXLINE 1024 // max text length for echo
#define SERV_PORT 10010 // required port number

/* GLOBAL VARIABLES */
int missingEchos[10000], n;
struct timeval sendTime, recvTime;
char numberString[6];
socklen_t serverLength;
struct sockaddr_in serverSocketAddress;

/* Trim trailing whitespace from the recieved message */
void trimTrailing(char * message)
{
    int i = 0;
    while(message[i] != ' ' && message[i] != 0)
    {
        i++;
    }

    message[i + 1] = 0;
}


/* Process 2: Recieves & logs the response from the server; reports missing echos */
void recieveResponse(socket, i) {
	char recieveMessage[MAXLINE];
	
	/* Recieve the response from the server; clock the end time to the millisecond */
	if (n = recvfrom(socket, recieveMessage, MAXLINE, 0, (struct sockaddr *) &serverSocketAddress, &serverLength) < 0) {
		perror("The server terminated prematurely.");
		exit(4);
	}
	gettimeofday(&recvTime, NULL);
	printf("%s", "String recieved from the server: ");
	printf("%s\n", recieveMessage);
	
	/* Check that the sent string matches the recieved string (check for missing echos) */
	trimTrailing(recieveMessage);
	numberString[snprintf(0, 0, "%+d", i) - 1] = 0;
	if (strcmp(recieveMessage, numberString) != 0) {
		missingEchos[i - 1] = 1;
	} 
	else {
		missingEchos[i - 1] = 0;
	}
}

/* Process 1: Sends the numbers 1 to 10000 as strings to the server & reports RTT statistics */
int main(int argc, char **argv) {
	int dataSocket, i, missingEcho, missingCount;
	struct hostent* serverHost;
	long RTT, highestRTT, lowestRTT, averageRTT, sumRTT;
	char sendMessage[MAXLINE];
	
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
		exit(2);
	}
	memset(&serverSocketAddress, 0, sizeof(serverSocketAddress));
	serverSocketAddress.sin_family = AF_INET;
	serverSocketAddress.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr*) serverHost->h_addr_list[0]))); // Convert number_dot string to binary
	serverSocketAddress.sin_port = htons(SERV_PORT); // Host byte order -> Network byte order
	serverLength = sizeof(serverSocketAddress);
	
	/* Send the numbers 1 to 10000 as strings */
	sumRTT = 0;
	for (i = 1; i <= 10000; i++) {
		/* Cast the loop value to a string & send it to the server; clock the start time to the millisecond */
		snprintf(numberString, sizeof(numberString), "%d", i);
		if (n = sendto(dataSocket, numberString, strlen(numberString), 0, (struct sockaddr *) &serverSocketAddress, serverLength) < 0) {
			perror("Message send error");
			exit(3);
		}
		gettimeofday(&sendTime, NULL);
		
		recieveResponse(dataSocket, i);
		
		/* Calculate milliseconds elapsed between start & end; update RTT statistics */
		RTT = (recvTime.tv_sec - sendTime.tv_sec) * 1000L + (recvTime.tv_usec - sendTime.tv_usec) / 1000;
		if (i == 1) {
			lowestRTT = RTT;
			highestRTT = RTT;
		}
		if (RTT > highestRTT) {
			highestRTT = RTT;
		}
		else if (RTT < lowestRTT) {
			lowestRTT = RTT;
		}
		sumRTT = (double)(sumRTT + RTT);
	}
	averageRTT = (double)(sumRTT / 10000);
	
	printf("\n%s", "-------OUTPUT STATISTICS-------");
	
	/* Print the string value of any missing echos */
	printf("\n%s", "Missing echos: ");
	missingEcho = 0;
	missingCount = 0;
	for (i = 1; i <= 10000; i++) {
		if (missingEchos[i - 1] == 1) {
			if (missingEcho == 1) {
				printf("%s", ", ");
			}
			printf("%d", i);
			missingEcho = 1;
			missingCount = 1;
		}
	}
	if (missingCount == 0) {
		printf("%s", "None");	
	}
	
	/* Print RTT statistics gathered using clockgettime() */
	printf("\n%s", "Average RTT: ");
	printf("%ld", averageRTT);
	printf("%s\n", " milliseconds");
	printf("%s", "Lowest RTT: ");
	printf("%ld", lowestRTT);
	printf("%s\n", " milliseconds");
	printf("%s", "Highest RTT: ");
	printf("%ld", highestRTT);
	printf("%s\n", " milliseconds");
	
	return(0);
}
