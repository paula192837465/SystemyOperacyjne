#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>


int main(int argc, char **argv)
{
    if(argc<4)
        perror("Too few args");

    int N = atoi(argv[4]);

    char buffer[N];

    int fd;
    FILE* file = fopen(argv[3], "r");
    FILE* queue = fopen(argv[1], "w");

    if(queue==NULL)
        perror("there is no queue");

    if(file==NULL)
        perror ("There is no file");


    char* to_send = calloc(N+3, sizeof(char));
    while(fread(buffer, sizeof(char), N,file)>0)
    {
        //printf("%d %s*\n",getpid(), buffer);
        sprintf(to_send, "%s_%d", buffer, atoi(argv[2]));
        //printf("Test: %s\n", to_send);
        fwrite(to_send, sizeof(char), N+2,queue);
        fflush(queue);
        
        sleep(1);
        memset(buffer, 0 , strlen(buffer));
    }
    
    fclose(queue);
    fclose(file);
    printf("Koniec");
    return 0;
}
