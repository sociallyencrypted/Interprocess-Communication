#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO1_NAME "Ass3FIFO"
#define FIFO2_NAME "NoWayAss3FIFO"

int main()
{
    int num, fd, fd2;

    mknod(FIFO1_NAME, S_IFIFO | 0666, 0);
    mknod(FIFO2_NAME, S_IFIFO | 0666, 0);
    // listen for a packet of 5 strings and 5 IDs then send back max ID
    int packetCount = 0;
    printf("Will I ever find a writer?...\n");
    fd = open(FIFO1_NAME, O_RDONLY);
    printf("Got one :D\n");
    printf("Opening a channel for acknowledgements and appreciations\n");
    fd2 = open(FIFO2_NAME, O_WRONLY);
    printf("Got the writer to read :)\n");
    while (packetCount < 10){
        // receive packets
        char** packets = (char**)malloc(5*sizeof(char*));
        char s[10];
        for (int i = 0; i < 5; i++){
            if ((num = read(fd, s, 10)) == -1)
                perror("read");
            else {
                s[num] = '\0';
                printf("stringListener: read %d bytes: \"%s\"\n", num, s);
            }
        }
        int IDs[5];
        int bytesToRead = (packetCount >= 2) ? 2 : 1;
        for (int i = 0; i < 5; i++){
            char s[3];
            // if packetCount > 2 then read 2 bytes else 1 byte
            if ((num = read(fd, s, bytesToRead)) == -1)
                perror("read");
            else {
                s[num] = '\0';
                printf("idListener: read %d bytes: \"%s\"\n", num, s);
                IDs[i] = atoi(s);
            }
        }
        // send max ID
        int maxID = IDs[4];
        char maxIDString[3];
        sprintf(maxIDString, "%d", maxID);
        //open fifo2 for writing
        if ((num = write(fd2, maxIDString, strlen(maxIDString))) == -1)
            perror("write");
        else{
            printf("ackSender: wrote %d bytes\n", num);
        }
        packetCount++;
    }
    close(fd);
    close(fd2);
    return 0;
}