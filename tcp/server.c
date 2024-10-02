/* Filename: server.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#define TO_FEC_FIFO "/tmp/myfifo1"
#define FROM_FEC_FIFO "/tmp/myfifo2"
#define TCP_LEN 24

int main (int argc, char *argv[]) {
	if (argc != 3) {
		puts("args: <dotted-quad IPv4> <porno>");
		exit(1);
	}

	int serverFD, clientFD, portno;
	struct sockaddr_in serverAddress, clientAddress;
	char clientIP[INET_ADDRSTRLEN];	// address string length

	serverFD = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFD < 0) {
		puts("socket-call failure");
		exit(1);
	}
	
	// prepare socket struct
	portno = atoi(argv[2]);		// arg-to-integer
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portno);
	serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
	memset(serverAddress.sin_zero, '\0', sizeof serverAddress.sin_zero);
	
	// bind to address
	if (bind(serverFD, (struct sockaddr*) &serverAddress, sizeof serverAddress) < 0) {
		puts("binding failed");
		exit(1);
	}
	
	// listen and accept incoming calls
	listen(serverFD, 1);		// allow one client
	socklen_t clientAddressLength = sizeof clientAddress;
	clientFD = accept(serverFD, (struct sockaddr*) &clientAddress, &clientAddressLength);
	if (clientFD < 0) {
		puts("accept failed");
		exit(1);
	} else {
		inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, sizeof clientIP);
		printf("Connected to %s\n", clientIP);
	}

	
	puts("Setting up the FIFOs"); 
	int fifoToFecFD, fifoFromFecFD, temp;
	if ((temp = mknod(TO_FEC_FIFO, S_IFIFO | 0640, 0)) < 0) {
		puts("Creation of to-fec-fifo failed");
		perror("mknod:");
	}	
	if ((temp = mknod(FROM_FEC_FIFO, S_IFIFO | 0640, 0)) < 0) {
		puts("Creation of from-fec-fifo failed");
		perror("mknod:");
	}

	char fifoFromFecBuf[80], fifoToFecBuf[80];
	int nread = 0;
	int nwrote = 0;
	
	puts("Waiting for reader of TO_FEC_FIFO...");
	if ((fifoToFecFD = open(TO_FEC_FIFO, O_WRONLY)) < 0) {
		puts("Failed to open to-fec-fifo");
		perror("open:");
	} else
		puts("To-fec fifo is opened");
	if ((fifoFromFecFD = open(FROM_FEC_FIFO, O_RDONLY)) < 0) {
		puts("Failed to open from-fec-fifo");
		perror("open:");
	} else
		puts("From-fec-fifo is opened");

	puts("Finally past the setup. Going into while(1)...");


	// send and receive
	char buffer[TCP_LEN + 1];
	int n;

	while(1) {
		memset(buffer, 0, sizeof buffer);
		memset(fifoFromFecBuf, 0, sizeof fifoFromFecBuf);
		memset(fifoToFecBuf, 0, sizeof fifoToFecBuf);

		n = recv(clientFD, buffer, sizeof buffer - 1, 0);
		if (n < 0) {
			puts("Client has probably closed its socket");
			break;
		} else {
			buffer[n - 1] = '\0';
			printf("Tcp-client sent %d bytes:\n\t \"%s\"\n", n, buffer);
		}
		
		/* Interpret commands and act on them */
		strcpy(fifoToFecBuf, buffer);
		puts("Waiting for reader...");
		if ((nwrote = write(fifoToFecFD, fifoToFecBuf, strlen(fifoToFecBuf))) < 0) {
			puts("write to FIFO_TO_FEC failed");
			perror("write:");
			break;
		} else {
			printf("We just wrote %d bytes to FIFO_TO_FEC\n", nwrote);
		}
		puts("Waiting for writer...");
		nread = read(fifoFromFecFD, fifoFromFecBuf, sizeof fifoFromFecBuf);
		if (nread < 0) {
			puts("read from FIFO_FROM_FEC failed");
			perror("read:");			
			break;
		} else {
			fifoFromFecBuf[nread] = '\0';
			printf("We just read %d bytes: \"%s\" from FIFO_FROM_FEC\n", nread, fifoFromFecBuf);
		}
		for (int i = 0; i < strlen(fifoFromFecBuf); i++)
			fifoFromFecBuf[i] = toupper(fifoFromFecBuf[i]);
		fifoFromFecBuf[strlen(fifoFromFecBuf)] = '\n';
		n = send(clientFD, fifoFromFecBuf, strlen(fifoFromFecBuf), 0);
		if (n < 0 ) {
			puts("Send failed");
			perror("send:");
			break;
		} else {
			printf("Server sent %d bytes\n", n);
		}
			
	}
	
	puts("Invalid command is given. Hence we close");
	close(clientFD);
	close(serverFD);
	close(fifoToFecFD);
	close(fifoFromFecFD);
	unlink(TO_FEC_FIFO);
	unlink(FROM_FEC_FIFO);
	return 0;
}
