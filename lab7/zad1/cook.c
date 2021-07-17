#include "utils.h"

struct sembuf lock_oven  = {AVAILABLE_OVEN, -1, 0};
struct sembuf add_to_oven = {OVEN_FREE_PLACES, -1, 0};
struct sembuf unlock_oven = {AVAILABLE_OVEN, 1, 0};
struct sembuf take_out_from_oven = {OVEN_FREE_PLACES, 1, 0};

struct sembuf lay_on_table = {TABLE_FREE_PLACES,-1, 0};
struct sembuf lock_table = {AVAILABLE_TABLE,-1, 0};
struct sembuf unlock_table = {AVAILABLE_TABLE,1, 0};
struct sembuf increase_pizzas_on_table = {PIZZAS_ON_TABLE, 1, 0};

int sem_id;
int shared_id;
pizzeria *pizza_shared_m;


int find_free_place_in_oven(int* arr)
{
    for(int i=0;i<OVEN_SIZE;i++)
    {
         if(arr[i]==-1)
        {
            return i;
        }
    }
       
    //printf("wychodzi");       
    return -1;
}

int find_free_place_on_table(int arr[])
{
    for(int i=0;i<TABLE_SIZE;i++)
        if(arr[i]==-1)
            return i;
    return -1;
}

void open_semafors()
{
    key_t key = ftok(getenv("HOME"),1);

    sem_id = semget(key, 0, PERMISSIONS);
   // printf(" sems-cook %d\n", sem_id);
    check(sem_id, "Error while creating semafors");
}

void open_shared_memory()
{
    key_t key = ftok(getenv("HOME"), 2);

    shared_id = shmget(key, sizeof(pizzeria) , PERMISSIONS);
    //printf("%d\n", shared_id);
    check(shared_id, "Error while opening shared memory");

    pizza_shared_m = shmat(shared_id, NULL, 0);
    if (pizza_shared_m == NULL)
            perror("shmat");

}

void ex()
{
    printf("KONIEC");
}

int main(int argc, char ** argv)
{
    srand(getpid());
    open_semafors();
    open_shared_memory();
    int pizza_type;
    atexit(ex);

    while(1)
    {
      
        //preparing pizza
        pizza_type = rand()%10;
        sleep(2);
        print_timestamp();
        printf("Pracownik %d przygotowuje pizze: %d\n", getpid(), pizza_type);

              
        //adding to the oven and waiting
        struct sembuf operations[2]= {lock_oven, add_to_oven};
        sem_operations(sem_id, operations, 2);

        int place_in_oven = find_free_place_in_oven(pizza_shared_m->oven);
        check(place_in_oven,"There is no place in oven");
        pizza_shared_m->oven[place_in_oven] = pizza_type;
        pizza_shared_m->pizzas_in_oven+=1;
        
        struct sembuf operations2[1] = {unlock_oven};
        sem_operations(sem_id, operations2, 1);
        print_timestamp();
        printf("Pracownik %d. Dodalem pizze: %d. Liczba pizz w piecu: %d\n", getpid(), pizza_type, pizza_shared_m->pizzas_in_oven);
        int time = rand()%2+4;
        printf("%d\n", time);
        sleep(time);

        //taking out pizza from the oven

        struct sembuf operations3[2] = {lock_oven, take_out_from_oven};
        sem_operations(sem_id, operations3, 2);

        pizza_shared_m->oven[place_in_oven]=-1;
        pizza_shared_m->pizzas_in_oven-=1;

        //placing pizza on the table
        struct sembuf operations4[4] = {unlock_oven, lock_table, lay_on_table, increase_pizzas_on_table};
        sem_operations(sem_id, operations4, 4);

        int place_table = find_free_place_on_table(pizza_shared_m->table);
        pizza_shared_m->table[place_table]=pizza_type;
        pizza_shared_m->pizzas_on_table +=1;

        struct sembuf operations5[1] = {unlock_table};
        sem_operations(sem_id, operations5, 1);

        print_timestamp();
        printf("Pracownik %d. Wyjmuje pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole%d\n", getpid(), pizza_type, pizza_shared_m->pizzas_in_oven, pizza_shared_m->pizzas_on_table);
        

        
    }
    shmdt(pizza_shared_m);
    return 0;
}