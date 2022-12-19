#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_SIZE 128

int main(int argc, char* argv[])
{
    if (argc > 2 || argc <= 1) {
        fprintf(stderr, "usage: P1shared [relative/path/to/key]\n");
        exit(1);
    }
    key_t key;
    int shmid;
    char *data;
    int mode;

    char* path = argv[1];
    if ((key = ftok(path, 'R')) == -1) {
        perror("ftok");
        exit(1);
    }

    /* connect to (and possibly create) the segment: */
    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    /* attach to the segment to get a pointer to it: */
    data = shmat(shmid, (void *)0, 0);
    if (data == (char *)(-1)) {
        perror("shmat");
        exit(1);
    }

    int packetCount = 0;
    while (packetCount < 10) {
        while (1){
            sleep(1);
            if (strcmp(data, "") != 0) {
                break;
            }
        }
        printf("P2: %s\n", data);
        // tokenise data to get last number in data
        char* token = strtok(data, " ");
        char* lastNumber = malloc(10);
        while (token != NULL) {
            strcpy(lastNumber, token);
            token = strtok(NULL, " ");
        }
        strncpy(data, "", SHM_SIZE);
        strcat(data, lastNumber);
        printf("lastNumber: %s\n", lastNumber);
        packetCount++;
    }
    

    /* detach from the segment: */
    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}