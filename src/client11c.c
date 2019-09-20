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
#include <limits.h>

#define MAXLINE 1024
#define SERV_PORT 10010
#define PACKET_SIZE 1038

/* GLOBAL VARIABLES */
static int missingEchos[10001];
static int n, sequenceFirst, sequenceLast, sequenceNumber, dataSocket;
static double packetDropRate;
struct timeval sendTime, recvTime;
socklen_t serverLength;
static long recvTimeMS, RTT, timestamp, highestRTT, lowestRTT, averageRTT, sumRTT;
static int sequence = -1;
static struct sockaddr_in serverSocketAddress;
static char packet[PACKET_SIZE], string[6];
static short totalMessageLength;

void createPacket() {
	sequenceNumber = htonl(0);
	totalMessageLength = htons(15 + strlen(string));
	memcpy(packet, &totalMessageLength, sizeof(short));
	memcpy(packet + sizeof(short), &sequenceNumber, sizeof(int));
	memcpy(packet + sizeof(short) + sizeof(int), &timestamp, sizeof(long));
	memcpy(packet + sizeof(short) + sizeof(int) + sizeof(long), &string, MAXLINE);
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

void printMissingEchoes() {	
	int packetsDropped = 0;
	int i;
	int printed = 0;
	printf("\n%s", "Missing echoes: ");
	for (i = 1; i < 10001; i++) {
		if (missingEchos[i] == 0) {
			packetsDropped++;
			if (printed == 1) {
				printf("%s", ", ");
			}
			printed = 1;
			printf("%d", i);
		}
	}
	packetDropRate = packetsDropped / (double)10000;
}

/* Process 2: Recieves & logs the response from the server; reports missing echos */
void recieveResponses() {
	int i = 0;
	int sequencePos = 1;
	int previousSequenceNumber = 0;
	while (n = recvfrom(dataSocket, packet, PACKET_SIZE, MSG_DONTWAIT, (struct sockaddr *) &serverSocketAddress, &serverLength) > 0) {
		/* Get current timestamp and get timestamp from the recv packet; calculate RTT */
		gettimeofday(&recvTime, NULL);
		recvTimeMS = (recvTime.tv_sec) * 1000L + (recvTime.tv_usec) / 1000;
		deconstructPacket();
		RTT = recvTimeMS - timestamp;
		sumRTT = sumRTT + RTT;
		
		printf("%s", "String recieved from the server: ");
		printf("%s\n", string);
		
		/* Set expected sequencenumber range & set current sequencenumber as recieved in the missingEchos array */
		if (sequence == -1) {
			sequence = 0;
			sequenceFirst = sequenceNumber;
			if (sequenceNumber - 10000 < UINT_MAX - 10000) {
				sequenceLast = sequenceNumber + 10000;
			}
			else {
				sequenceLast = 10000 - (UINT_MAX - sequenceNumber);
			}
		}
		sscanf(string, "%d", &sequencePos);
		missingEchos[sequencePos] = 1;
		previousSequenceNumber = sequenceNumber;
		
		/* Update RTT statistics */
		if (i == 0) {
			lowestRTT = RTT;
			highestRTT = RTT;
		}
		if (RTT > highestRTT) {
			highestRTT = RTT;
		}
		if (RTT < lowestRTT) {
			lowestRTT = RTT;
		}
		
		i++;
	}
	averageRTT = sumRTT / i;
	
	printMissingEchoes();
}

/* Process 1: Sends the numbers 1 to 10000 as strings to the server & reports RTT statistics */
int main(int argc, char **argv) {
	int i, missingEcho, missingCount;
	struct hostent* serverHost;
	struct timeval sendTime, recvTime;
	
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
	serverSocketAddress.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr*) serverHost->h_addr_list[0])));
	serverSocketAddress.sin_port = htons(SERV_PORT);
	serverLength = sizeof(serverSocketAddress);
	
	/* Initialize all missingEchos to 0; i.e, all echos are missing before send */
	for (i = 0; i < 10000; i++) {
		missingEchos[i] = 0;
	}
	
	/* Send the numbers 1 to 10000 as strings */
	for (i = 1; i <= 10000; i++) {
		/* Cast the loop value to a string & send it to the server; clock the start time to the millisecond */
		snprintf(string, sizeof(string), "%d", i);
		
		/* Get current timestamp and format for transport */
		gettimeofday(&sendTime, NULL);
		timestamp = htobe64((sendTime.tv_sec) * 1000L + (sendTime.tv_usec) / 1000);
		
		/* Create and send the packet to the server */
		createPacket();
		if (n = sendto(dataSocket, packet, ntohs(totalMessageLength), 0, (struct sockaddr *) &serverSocketAddress, serverLength) < 0) {
			perror("Message send error");
			exit(3);
		}
	}
	
	recieveResponses(dataSocket);
	
	/* Print Drop Rate & RTT statistics gathered using gettimeofday() */
	printf("\n%s", "-------DROP RATE & RTT STATISTICS-------");
	printf("\n%s", "Packet Drop Rate: %");
	printf("%0.2f", packetDropRate * 100);
	printf("\n%s", "Average RTT: ");
	printf("%ld", averageRTT);
	printf("%s\n", " millisecond(s)");
	printf("%s", "Lowest RTT: ");
	printf("%ld", lowestRTT);
	printf("%s\n", " millisecond(s)");
	printf("%s", "Highest RTT: ");
	printf("%ld", highestRTT);
	printf("%s\n", " millisecond(s)");
	
	return(0);
}