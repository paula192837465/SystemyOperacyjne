#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<stdbool.h>
#include <time.h>

#define ELVES_NUMBER 10
#define REINDEER_NUMBER 9
#define REINDEER_TIME_OF_SLEEP rand()%5+5
#define ELVES_TIME_OF_WORKING rand()%3+2
#define SANTA_DELIVERY_TIME rand()%2+2
#define SANTA_HELPING_TIME rand()%2+1
#define and &&
#define or ||


int deliveries =0;
int elves_waiting =0;
int reindeers_waiting =0;
int elves_waiting_arr[3]={};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_ready = PTHREAD_COND_INITIALIZER;
pthread_cond_t santa_finished = PTHREAD_COND_INITIALIZER;

void* santa_thread(void *arg)
{
    while(true)
    {
        if(deliveries==3)
            break;
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&santa_ready, &mutex);
        pthread_mutex_unlock(&mutex);

        printf("Santa's awake!\n");
        printf("Mikołaj: czeka %d reniferów oraz %d elfów\n", reindeers_waiting, elves_waiting);

        pthread_mutex_lock(&mutex);
        if(reindeers_waiting==REINDEER_NUMBER and elves_waiting==3)
        {
            printf("Mikołaj: dostarczam zabawki\n");
            sleep(SANTA_DELIVERY_TIME);
            reindeers_waiting =0;
            deliveries++;

            printf("Mikołaj: rozwiązuje problemy elfów %d %d %d\n", elves_waiting_arr[0], elves_waiting_arr[1], elves_waiting_arr[2]);
            sleep(SANTA_HELPING_TIME);
            elves_waiting=0;
            for(int i=0;i<3;i++)
                elves_waiting_arr[i]=0;
        }
        else if(reindeers_waiting==REINDEER_NUMBER )
        {
            printf("Mikołaj: dostarczam zabawki\n");
            sleep(SANTA_DELIVERY_TIME);
            reindeers_waiting =0;
            deliveries++;
        }
        else if(elves_waiting==3)
        {
            printf("Mikołaj: rozwiązuje problemy elfów %d %d %d\n", elves_waiting_arr[0], elves_waiting_arr[1], elves_waiting_arr[2]);
            sleep(SANTA_HELPING_TIME);
            elves_waiting=0;
            for(int i=0;i<3;i++)
                elves_waiting_arr[i]=0;
        }
        printf("Mikołaj: zasypiam\n");
         pthread_mutex_unlock(&mutex);
    }
    exit(0);
}

void* elf_thread(void *arg)
{
    int elf_id = *((int*)arg);

    while(true)
    {
        if(deliveries==3)
            break;
        sleep(ELVES_TIME_OF_WORKING);
        
        pthread_mutex_lock(&mutex);
        elves_waiting++;

        if(elves_waiting<=3){
            elves_waiting_arr[elves_waiting-1]=elf_id;
            printf("Elf: czeka %d elfów na Mikołaja. Elf %d\n", elves_waiting, elf_id);
        
            if(elves_waiting==3)
            {
            printf("Elf: wybudzam Mikołaja %d\n", elf_id);
            pthread_cond_broadcast(&santa_ready);
            for(int i=0;i<3;i++)
                printf("Elf: Mikołaj rozwiązuje problem Elfa %d\n", elves_waiting_arr[i]);
            }
        }
        else if(elves_waiting>3){
            printf("Elf: czeka na powrót elfów. Elf %d\n", elf_id);
            elves_waiting=3;
        }

        pthread_mutex_unlock(&mutex);
       
    } 
    exit(0);
}

void* reindeer_thread(void *arg)
{
    int reindeer_id = *((int*)arg);

    while(true)
    {
        if(deliveries>=3)
            break;

        sleep(REINDEER_TIME_OF_SLEEP);

        pthread_mutex_lock(&mutex);
        reindeers_waiting++;
        printf("Renifer: czeka %d reniferów na Mikołaja. Renifer %d\n", reindeers_waiting, reindeer_id);
        if(reindeers_waiting==REINDEER_NUMBER)
        {
            printf("Renifer: wybudzam Mikołaja. Id: %d\n", reindeer_id);
            pthread_cond_broadcast(&santa_ready);
        }
        pthread_mutex_unlock(&mutex);
    }
    exit(0);
}

int main(int argc, int **argv)
{
    srand(time(NULL));

    pthread_t* santa = calloc(1, sizeof(pthread_t));
    pthread_t* elves = calloc(ELVES_NUMBER, sizeof(pthread_t));
    pthread_t* reindeers = calloc(REINDEER_NUMBER, sizeof(pthread_t));

    int elves_idx[ELVES_NUMBER];
    int reindeers_idx[REINDEER_NUMBER];

    for(int i=0;i<ELVES_NUMBER;i++)
        elves_idx[i]=i+1;

    for(int i=0;i<REINDEER_NUMBER;i++)
        reindeers_idx[i]=i+1;

    pthread_create(&santa, NULL, &santa_thread, NULL);

    for(int i=0;i<ELVES_NUMBER;i++)
        pthread_create(&elves[i], NULL, &elf_thread, &elves_idx[i]);

    for(int i=0;i<REINDEER_NUMBER;i++)
        pthread_create(&reindeers[i], NULL, &reindeer_thread, &reindeers_idx[i]);


    if(pthread_join(santa, NULL)!=0)
        perror("Error with terminating Santa's thread\n");

    for (int i = 0; i < REINDEER_NUMBER; i++) {
        if (pthread_join(reindeers[i],NULL) != 0) 
            perror("Failed to join thread");
    }

    return 0;
}