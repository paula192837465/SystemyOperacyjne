#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>
#include <sys/times.h>
#include <dlfcn.h>
#include <ctype.h>
#include <time.h>


double timeDifference(clock_t t1, clock_t t2)
{
    return ((double)(t2 - t1) / sysconf(_SC_CLK_TCK));
}

void saveResults(clock_t start, clock_t end, struct tms* t_start, struct tms* t_end)
{
     
    printf("\tREAL_TIME: %fl\n", timeDifference(start,end));
    printf("\tUSER_TIME: %fl\n", timeDifference(t_start->tms_utime, t_end->tms_utime));
    printf("\tSYSTEM_TIME: %fl\n", timeDifference(t_start->tms_stime, t_end->tms_stime));

    FILE* res= fopen("pomiar_zad_4a.txt", "w+");
    char buffer [50];
    int n;
    n=sprintf (buffer, "\tREAL_TIME: %fl\n", timeDifference(start,end));
    fwrite(buffer,sizeof(char),n, res);
    n=sprintf (buffer, "\tUSER_TIME: %fl\n", timeDifference(t_start->tms_utime, t_end->tms_utime));
    fwrite(buffer,sizeof(char),n, res);
    n=sprintf (buffer, "\tSYSTEM_TIME: %fl\n",timeDifference(t_start->tms_stime, t_end->tms_stime));
    fwrite(buffer,sizeof(char),n, res);

    fclose(res);

}

int getLine(FILE* file, FILE* resultFile, char* word1, char* word2)
{
    char *letter = calloc(1, sizeof(char));
    int counter =0, readed, exists=0;
    char * line = calloc(1, sizeof(char));

    while(readed= fread(letter, sizeof(char), 1, file)==1)
    {
        if(strcmp(letter,"\n")==0 || strcmp(letter," ")==0 || strcmp(letter,".")==0 || strcmp(letter,",")==0 || strcmp(letter,":")==0 )
        {
            line = realloc(line,  counter+1);
            break;
        }

        line = realloc(line, sizeof(char)*strlen(line)+1);
        strcat(line, letter);
    }

    if(readed==0)
        return -1;

    if(strcmp(line, word1)==0)
    {
        line = realloc(line, sizeof(word2)+1);
        strcpy(line, word2);
        strcat(line,letter);
        fwrite(line, sizeof(char), strlen(line), resultFile);
    }
    else
    {
        strcat(line,letter);
        fwrite(line, sizeof(char), strlen(line), resultFile);
    }
    
    free(letter);
    free(line);
    return 1;
}
void searchFile(FILE * A, FILE* result, char * word1, char * word2)
{
    int startA;
    startA=0;

    while(startA!=-1)
        startA=getLine(A, result,  word1, word2);

    fclose(A);
    fclose(result);

}
int main(int argc, char** argv) {

    struct tms tmsTotalStart, tmsTotalEnd;
    clock_t totalStart, totalEnd;

    totalStart = times(&tmsTotalStart);

    if(argc<4)
        perror("to few arguments");
    FILE* fileA = fopen(argv[1],"r");
    FILE* fileB = fopen(argv[2], "w");
    if(fileA==NULL || fileB==NULL)
    {
        perror("There is no such a file");
        return 0;
    }
    char* arr[2] = {argv[3], argv[4]};
    searchFile(fileA, fileB,arr[0], arr[1]);

    totalEnd = times(&tmsTotalEnd);
    saveResults(totalStart, totalEnd, &tmsTotalStart, &tmsTotalEnd);

    return 0;
}