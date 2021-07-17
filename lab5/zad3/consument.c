#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

//get a number from string
int extractNumber(char * line)
{
    int num;
    if (1 == sscanf(line, "%*[^0123456789]%d", &num))
    {
        //printf("%d\n", num);
        return num;
    }
}


int main(int argc, char **argv)
{
    FILE* queue = fopen(argv[1], "r");

    int N=atoi(argv[3]);

    if(queue==NULL)
        perror("There is no queue file");
    

    char buffor[N];
    int i=0, numOfLine;

    while(fread(buffor, sizeof(char), N,queue)==N)
    {
        printf("Test: %d %s\n", i, buffor);
        numOfLine = extractNumber(buffor);
        i++;
        memset(buffor, 0 , strlen(buffor));
        fflush(stdout);

    }
    fclose(queue);
    printf("konuec");
    return 0;
}
