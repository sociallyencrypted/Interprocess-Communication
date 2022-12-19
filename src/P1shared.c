#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_SIZE 128

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

    char** strings = generateRandomStrings(50, 10);
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
        strncpy(data, "", SHM_SIZE);
        int baseStringNumber = packetCount*5;
        for (int i = 0; i < 5; i++) {
            strcat(data, strings[baseStringNumber + i]);
            strcat(data, " ");
        }
        int IDs[5];
        for (int i = 0; i < 5; i++) {
            IDs[i] = baseStringNumber + i;
            char* ID = (char*)malloc(10*sizeof(char));
            sprintf(ID, "%d", IDs[i]);
            strcat(data, ID);
            strcat(data, " ");
        }
        packetCount++;
        int maxStringNumber = baseStringNumber + 4;
        char* maxStringNumberString = (char*)malloc(10*sizeof(char));
        sprintf(maxStringNumberString, "%d", maxStringNumber);
        while (1){
            if (strcmp(data, maxStringNumberString) == 0) {
                strncpy(data, "", SHM_SIZE);
                break;
            }
        }
    }
    

    /* detach from the segment: */
    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}