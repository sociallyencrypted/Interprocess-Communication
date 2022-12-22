//P2 using unix sockets

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "Ass3socket"

int main(void)
{
    int s, t, len;
    struct sockaddr_un remote;
    char str[100];

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    printf("Trying to connect...\n");

    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, SOCKET_PATH);
    len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        perror("connect");
        exit(1);
    }

    printf("Connected.\n");

    // recieve 5 strings and send an ack
    int packetCount = 0;
    while (1) {
        int n;
        for (int i=0; i<5; i++){
            if ((n = recv(s, str, 10, 0)) > 0) {
                str[n] = '\0';
                printf("Received: %s\n", str);
            } else {
                if (n < 0) perror("recv");
                else printf("Server closed connection\n");
                exit(1);
            }
        }
        // recieve IDs
        int IDs[5];
        for (int i=0; i<5; i++){
            if ((n = recv(s, str, 10, 0)) > 0) {
                str[n] = '\0';
                IDs[i] = atoi(str);
                printf("Received ID: %d\n", IDs[i]);
            } else {
                if (n < 0) perror("recv");
                else printf("Server closed connection\n");
                exit(1);
            }
        }
        // send ack
        int maxID = IDs[0];
        for (int i=1; i<5; i++){
            if (IDs[i] > maxID) maxID = IDs[i];
        }
        printf("Sending ack: %d\n", maxID);
        if (send(s, str, strlen(str), 0) == -1) {
            perror("send");
            exit(1);
        }
        packetCount++;
        if (packetCount == 10) break;
    }

    close(s);

    return 0;
}