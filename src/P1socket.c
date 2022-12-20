// Write two programs P1 and P2. The first program P1 needs to generate an
// array of 50 random strings (of characters) of fixed length each. P1 then sends
// a group of five consecutive elements of the array of strings to P2 along with
// the ID’s of the strings, where the ID is the index of the array corresponding
// to the string. The second program P2 needs to accept the received strings,
// and send back the highest ID received back to P1 to acknowledge the strings
// received. The program P2 simply prints the ID’s and the strings on the console.
// On receiving the acknowledged packet, P1 sends the next five strings, with the
// string elements starting from the successor of the acknowledged ID.

//count time taken for all acks to be received

//P1 using unix socket

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

#define SOCKET_PATH "Ass3socket"

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
    int s, s2, t, len;
    struct sockaddr_un local, remote;
    char str[100];

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCKET_PATH);
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(s, (struct sockaddr *)&local, len) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(s, 1) == -1) {
        perror("listen");
        exit(1);
    }

    while(1) {
        int done, n;
        printf("Waiting for a connection...\n");
        t = sizeof(remote);
        if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
            perror("accept");
            exit(1);
        }

        printf("Connected.\n");
        char** arr = generateRandomStrings(50, 10);
        done = 0;
        int packetCount = 0;
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        do {
            for (int i = 0; i < 5; i++)
            {
                int baseStringNumber = packetCount*5;
                if (send(s2, arr[baseStringNumber + i], 10, 0) < 0) {
                    perror("send");
                    done = 1;
                }
            }
            // send IDs
            for (int i = 0; i < 5; i++)
            {
                int baseStringNumber = packetCount*5;
                char* baseStringNumberStr = (char*)malloc(10*sizeof(char));
                sprintf(baseStringNumberStr, "%d", baseStringNumber + i);
                if (send(s2, baseStringNumberStr, 10, 0) < 0) {
                    perror("send");
                    done = 1;
                }
            }
            // recieve ack to send next 5
            if (recv(s2, str, 100, 0) < 0) {
                perror("recv");
                done = 1;
            }
            //print recieved ack and compare with max recieved
            char* maxStringNumber = (char*)malloc(10*sizeof(char));
            sprintf(maxStringNumber, "%d", packetCount*5+4);
            if (strcmp(str, maxStringNumber) != 0){
                printf("Expected ack: %s\n", maxStringNumber);
                printf("Ack invalid\n");
                done = 1;
            }
            packetCount++;
            if (packetCount == 10){
                clock_gettime(CLOCK_MONOTONIC, &end);
                printf("All acks recieved in %f seconds\n", (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec)/1000000000.0);
                done = 1;
            }
        } while (!done);

        close(s2);
    }

    return 0;
}