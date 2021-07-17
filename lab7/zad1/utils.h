#ifndef utils_h
#define utils_h

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define TABLE_SIZE 5
#define OVEN_SIZE 5
#define PROJEC_ID "p"
#define PERMISSIONS 0666
#define PATH "HOME"


#define repeat(n) for(int i=0;i<n;i++)
#define spawn(prog) if(fork()==0 && execlp(prog, prog, NULL) == -1) exit(1))

typedef enum Semafors {OVEN_FREE_PLACES, TABLE_FREE_PLACES, AVAILABLE_OVEN, AVAILABLE_TABLE, PIZZAS_ON_TABLE} sem_no;

typedef struct pizzeria {
    int oven[OVEN_SIZE];
    int table[TABLE_SIZE];
    int pizzas_in_oven;
    int pizzas_on_table;
} pizzeria;


void print_timestamp() {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    printf("(PID: %d time: %ld) ", getpid(),
           spec.tv_sec * 1000 + spec.tv_nsec / 1000000);
}

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo *__buf;
};


void check(int output, char * error) {
    if(output<0)
        perror(error);
}

void sem_operations(int sem_nr, struct sembuf *sembuff, int operations_num)
{
    for(int i=0;i<operations_num;i++)
        if(semop(sem_nr, &sembuff[i],1)==-1)
            perror("Error while executing operation");
    
}

#endif