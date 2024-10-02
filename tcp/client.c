/* Filename: client.c */
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define TO_FEC_FIFO  "/tmp/myfifo1"
#define FROM_FEC_FIFO "/tmp/myfifo2"

int main() 
{
	int fifoToFecFD, fifoFromFecFD;
	int stringlen, readlen, writelen;
	char readbuf[80], writebuf[80];
	
	fifoToFecFD = open(TO_FEC_FIFO, O_RDONLY);
	fifoFromFecFD = open(FROM_FEC_FIFO, O_WRONLY);

	while (1) {
		memset(readbuf, '\0', sizeof(readbuf));
		memset(writebuf, '\0', sizeof(writebuf));

         if ((readlen = read(fifoToFecFD, readbuf, sizeof(readbuf))) < 0) {
			puts("Reading from \"TO_FEC_FIFO\" failed");
			perror("read:");
		} else {
			readbuf[readlen] = '\0';
			printf("Read the string \"%s\" from \"TO_FEC_FIFO\". %d bytes\n", readbuf, readlen);
		}
		printf("Enter string to send from FEC:\n\t");
      		fgets(readbuf, sizeof(readbuf), stdin);
      		stringlen = strlen(readbuf);
      		readbuf[stringlen - 1] = '\0';

		if ((writelen = write(fifoFromFecFD, readbuf, strlen(readbuf))) < 0) {
			puts("writing to \"FROM_FEC_FIFO\" failed");
			perror("write:");
		} else {
			printf("Sent string \"%s\" with length %d\n", readbuf, writelen); 
         	}
		
   	}
	
	return 0;
}
