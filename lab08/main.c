#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

int **image_normal;
int **image_negative;
int W, H;
int thread_num;

double get_time(struct timespec *start) {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    return (end.tv_sec - start->tv_sec) * 1e6 + (end.tv_nsec - start->tv_nsec) / 1e3;
}

void* numbers_version(void* arg)
{
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int index = *((int*)arg);

    int range_min = ceil(255/thread_num)*index;
    int range_max = fmin((range_min+ ceil((double) 256 / thread_num) -  1), 255);

    for(int i=0;i<W;i++)
        for(int j=0;j<H;j++)
        {
            int temp_pixel = image_normal[i][j];
            if(temp_pixel>=range_min && temp_pixel<=range_max)
                 image_negative[i][j]= 255-temp_pixel;
        }

    double *time = calloc(1, sizeof(double));
    *time = get_time(&start);
    return time;
}

void* blocks_version(void* arg)
{
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int index = *((int*)arg);

    int range_min = ceil(W/thread_num)*index;
    int range_max = fmin((range_min + ceil((double) W/ thread_num) -  1), W - 1);


    for(int i=0;i<W;i++)
        for(int j=0;j<H;j++)
        {
            int temp_pixel = image_normal[i][j];
            if(i>=range_min && i<=range_max)
                 image_negative[i][j]= 255-temp_pixel;
        }

    double *time = calloc(1, sizeof(double));
    *time = get_time(&start);
    return time;
}

int main(int argc, char **argv)
{
    if(argc<5)
        perror("Too few args");

    thread_num = atoi(argv[1]);

    FILE* image = fopen(argv[3], "r");
    FILE* image_res = fopen(argv[4], "w");

    FILE *times = fopen("Times.txt", "a");
    fprintf(times, "Number of threads: %d \t Mode: %s\n", thread_num, argv[2]);

    if(image ==NULL)
        perror("Error while opening image");

    //CZYTANE DANYCH Z PLKU WEJSCIOWEGO

    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    char header[10];
    fgets(header, sizeof(header), image);
    printf("%s\n", header);

    char header2[50];
    fgets(header2, sizeof(header2), image);
    printf("%s\n", header2);

    fscanf(image, "%d %d", &W, &H);
    printf("%d %d\n", W,H);

    int pixels;

    fscanf(image, "%d", &pixels);
    printf("%d\n", pixels);

    //ALOKACJA PAMIECI
    image_normal = calloc(W, sizeof(int*));
    image_negative =calloc(W, sizeof(int*));

    for(int i=0;i<W; i++)
    {
        image_normal[i] = calloc(H, sizeof(int));
        image_negative[i] = calloc(H, sizeof(int));
    }

    for(int i =0;i<W; i++)
        for(int j=0;j<H;j++)
        {
            fscanf(image, "%d", &image_normal[i][j]);
        }

    pthread_t* threads = calloc(thread_num+1, sizeof(pthread_t));

    int indexes[thread_num];
    for(int i=0;i<thread_num;i++)
        indexes[i]=i;

    //WYWOLANIE N WATKOW
    for(int i=0;i<thread_num;i++)
    {
        
        if(strcmp(argv[2], "numbers")==0)
            pthread_create(&threads[i], NULL, &numbers_version, &indexes[i]);
        if(strcmp(argv[2], "blocks")==0)
            pthread_create(&threads[i], NULL, &blocks_version, &indexes[i]);
    
    }

    //OCZEKIWANIE NA WSZYTSKIE WATKI
    for (int i = 0; i < thread_num; i++) {
        double *result;
        if (pthread_join(threads[i],  (void *)&result) != 0) {
            perror("Failed to join thread");
        }
         printf("Thread %d \t %.3f microseconds\n", i + 1, *result);
         fprintf(times, "Thread %d \t %.4f microseconds\n", i + 1, *result);
         free(result);
    }
       
    double full_time = get_time(&start);
    printf("\nFull time %.3f microseconds\n--------------------------------\n\n", full_time);
    fprintf(times, "\nFull time %.4f microseconds\n----------------------------\n\n", full_time); 
    fclose(times);

    //WCZYTYWANIE DO NOWEGO PLIKU 
    fprintf(image_res, "P2\n");
    fprintf(image_res, "%d %d\n", W, H);
    fprintf(image_res, "%d\n", 255);
    for(int i =0;i<W; i++)
    {
        for(int j=0;j<H;j++)
        {
            fprintf(image_res, "%d ", image_negative[i][j]);
        }
        fprintf(image_res, "\n");
    }
       

    return 0;
}