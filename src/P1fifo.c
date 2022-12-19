#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define FIFO1_NAME "Ass3FIFO"
#define FIFO2_NAME "NoWayAss3FIFO"

char** generateRandomStrings(int n, int len)
{
    char** strings = (char**)malloc(n*sizeof(char*));
    for(int i=0; i<n; i++)
    {
        strings[i] = (char*)malloc(len*sizeof(char));
        for(int j=0; j<len; j++)
        {
            strings[i][j] = 'a' + rand()%26;
        }
        strings[i][len] = '\0';
    }
    return strings;
}

int main()
{
    char s[300];
    int num, fd, fd2;

    mknod(FIFO1_NAME, S_IFIFO | 0666, 0);
    mknod(FIFO2_NAME, S_IFIFO | 0666, 0);

    int packetCount = 0;
    char** packets = generateRandomStrings(50, 10);
    struct timespec start, end;
    printf("Opening packetSender FIFO\n");
    fd = open(FIFO1_NAME, O_WRONLY);
    printf("Got the reader :)\n");
    printf("Opening ackReciever FIFO\n");
    fd2 = open(FIFO2_NAME, O_RDONLY);
    printf("Caught ya!\n");
    printf("Releasing packets\n");
    clock_gettime(CLOCK_REALTIME, &start);
    while (packetCount < 10){
        // send packets
        int baseStringNumber = packetCount*5;
        for (int i = 0; i < 5; i++){
            if ((num = write(fd, packets[baseStringNumber+i], strlen(packets[baseStringNumber + i]))) == -1){
                perror("write");
            }
        }
        // send IDs
        int IDs[5];
        for (int i = 0; i < 5; i++){
            IDs[i] = baseStringNumber + i;
            char ID[3];
            sprintf(ID, "%d", IDs[i]);
            if ((num = write(fd, ID, strlen(ID))) == -1){
                perror("write");
            }
        }
        // recieve ack through FIFO2

        do {
            if ((num = read(fd2, s, 300)) == -1)
                perror("read");
            else {
                s[num] = '\0';
                int maxStringNumber = packetCount*5 + 4;
                if (atoi(s) == maxStringNumber){
                    break;
                }
                else{
                    printf("ackListener: received ack %d, but expected ack %d\n", atoi(s), maxStringNumber);
                    exit(1);
                }
            }
        } while (num > 0);
        packetCount++;
    }
    clock_gettime(CLOCK_REALTIME, &end);
    printf("Recieved all acks in %f seconds\n", (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec)/1000000000.0);
    close (fd);
    close(fd2);
    return 0;
}