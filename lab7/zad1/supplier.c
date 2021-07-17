#include "utils.h"

int sem_id;
int shared_id;
pizzeria *pizza_shared_m;


int find_pizza_on_table(int arr[])
{
    for(int i=0;i<TABLE_SIZE;i++)
        if(arr[i]!=-1)
            return i;
    return -1;
}

void open_semafors()
{
    key_t key = ftok(getenv("HOME"), 1);

    sem_id = semget(key, 0, PERMISSIONS);
    printf("sems-supp %d\n", sem_id);
    check(sem_id, "Error while creating semafors");
}

void open_shared_memory()
{
    key_t key = ftok(getenv("HOME"), 2);

    shared_id = shmget(key, sizeof(pizzeria) , PERMISSIONS);
    //printf("shared %d\n", shared_id);
    check(shared_id, "Error while opening shared memory");

    pizza_shared_m = shmat(shared_id, NULL, 0);
    if (pizza_shared_m == NULL)
            perror("shmat");
}

struct sembuf take_out_from_table = {TABLE_FREE_PLACES,1, 0};
struct sembuf lock_table = {AVAILABLE_TABLE, -1, 0};
struct sembuf unlock_table = {AVAILABLE_TABLE, 1, 0};
struct sembuf decrease_pizzas_on_table = {PIZZAS_ON_TABLE, -1, 0};
struct sembuf increase_pizzas_on_table = {PIZZAS_ON_TABLE, 1, 0};


int main(int argc, char ** argv)
{

    open_semafors();
    open_shared_memory();

    while(1)
    {

        struct sembuf operations[3] = {decrease_pizzas_on_table, lock_table, take_out_from_table};
        sem_operations(sem_id, operations, 3);

        int place_table = find_pizza_on_table(pizza_shared_m->table);
        int pizza_type =  pizza_shared_m->table[place_table];
        pizza_shared_m->table[place_table]=-1;
        pizza_shared_m->pizzas_on_table-=1;

        struct sembuf operations2[1] = {unlock_table};
        sem_operations(sem_id, operations2, 1);

        print_timestamp();
        printf("Pracownik %d Pobieram pizze: %d. Liczba pizz na stole%d\n", getpid(), pizza_type, pizza_shared_m->pizzas_on_table);
        int time = rand()%2+4;
        sleep(time);

        print_timestamp();
        printf("Pracownik %d dostarczam pizze: %d\n", getpid(), pizza_type);
        time = rand()%2+4;
        sleep(time);
    }

    shmdt(pizza_shared_m);
    return 0;

}