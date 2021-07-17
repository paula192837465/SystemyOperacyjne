#include "utils.h"

int sem_id;
int shared_id;
int cooks;
int suppliers;


void close_sem() { semctl(sem_id, 0, IPC_RMID); }

void close_shm() { shmctl(shared_id, IPC_RMID, NULL); }

void init_semafors()
{
    key_t key = ftok(getenv("HOME"), 1);

    sem_id = semget(key, 5 , IPC_CREAT | PERMISSIONS);
    //printf("sems %d\n", sem_id);
    check(sem_id, "Error while creating semafors");


    if(semctl(sem_id, OVEN_FREE_PLACES, SETVAL, (union semun){.val=OVEN_SIZE})==-1)
        perror("Errors with initializing semafors");
    if(semctl(sem_id, TABLE_FREE_PLACES, SETVAL, (union semun){.val=TABLE_SIZE})==-1)
        perror("Errors with initializing semafors");
    if(semctl(sem_id, AVAILABLE_OVEN, SETVAL, (union semun){.val=1})==-1)
        perror("Errors with initializing semafors");
    if(semctl(sem_id, AVAILABLE_TABLE, SETVAL, (union semun){.val=1})==-1)
        perror("Errors with initializing semafors");
    if(semctl(sem_id, PIZZAS_ON_TABLE, SETVAL, (union semun){.val=0})==-1)
        perror("Errors with initializing semafors");

}

void init_shared_memory()
{
    key_t key = ftok(getenv("HOME"), 2);

    shared_id = shmget(key, sizeof(pizzeria) , IPC_CREAT | PERMISSIONS);
    //printf("shared %d\n", shared_id);
    check(shared_id, "Error while opening shared memory");

    pizzeria *pizza_shared_m = shmat(shared_id, NULL, 0);
    if (pizza_shared_m == NULL)
            perror("shmat");

    for(int i =0;i<OVEN_SIZE;i++)
        pizza_shared_m->oven[i]=-1;

    for(int i =0;i<TABLE_SIZE;i++)
        pizza_shared_m->table[i]=-1;

    pizza_shared_m->pizzas_in_oven=0;
    pizza_shared_m->pizzas_on_table=0;
    
    shmdt(pizza_shared_m);

}

int main(int argc, char **argv)
{
    if(argc<3)
        perror("To few args");
     
    
    int cooks = atoi(argv[1]);
    int suppliers = atoi(argv[2]);
    char * shared_pizzeria;
    
    init_semafors();
    init_shared_memory();

    pid_t child;
    for(int i=0;i<cooks;i++)
    {
        printf("cooks explosion\n");
        child=fork();
        if(child==0)
            execlp("./cook", "cook", NULL);
        else if (child==-1)
            perror("Error while forking - cook");
    }

    for(int i=0;i<suppliers;i++)
    {
        child=fork();
        if(child==0)
            execlp("./supplier", "supplier", NULL);
        else if (child==-1)
            perror("Error while forking - supplier");
    }

    while(wait(NULL) >0);


    shmdt(shared_pizzeria);

    return 0;
}
