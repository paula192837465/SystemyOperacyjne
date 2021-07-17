#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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

    FILE* res= fopen("pomiar_zad_3a.txt", "w+");
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

int getLine(FILE* file, FILE* resA, FILE* resB, FILE* resC)
{
    char *letter = calloc(1, sizeof(char));
    int counter =0, readed;
    char * line = calloc(1, sizeof(char));

    while(readed= fread(letter, sizeof(char), 1, file)==1)
    {
        if(strcmp(letter,"\n")==0)
        {
            line = realloc(line,  counter+1);
            break;
        }
        line = realloc(line, sizeof(char)*strlen(line)+1);
        strcat(line, letter);
        counter++;
    }
    if(readed==0)
        return -1;

    strcat(line,letter);

    if(atoi(line)%2==0)   
        fwrite(line, sizeof(char), strlen(line), resA);
    
    if(strlen(line)>3 && (atoi(line)/10%10==7 || atoi(line)/10%10==0))
        fwrite(line, sizeof(char), strlen(line), resB);
    

    int temp= sqrt(atoi(line));
    if(temp*temp == atoi(line))
        fwrite(line, sizeof(char), strlen(line), resC);
    
       
    free(letter);
    free(line);
    return 1;
}
void searchFile(FILE * A)
{

    int startA;
    FILE* fileA = fopen("a.txt", "w");
    FILE* fileB = fopen("b.txt", "w");
    FILE* fileC = fopen("c.txt", "w");
    startA=0;

    while(startA!=-1)
        startA=getLine(A, fileA, fileB, fileC);

    fclose(A);
    fclose(fileA);
    fclose(fileB);
    fclose(fileC);

}
int main() {

    struct tms tmsTotalStart, tmsTotalEnd;
    clock_t totalStart, totalEnd;

    totalStart = times(&tmsTotalStart);

    FILE* fileA = fopen("dane.txt","r");
    if(fileA==NULL)
    {
        perror("There is no such a file");
        return 0;
    }
    searchFile(fileA);

    totalEnd = times(&tmsTotalEnd);
    saveResults(totalStart, totalEnd, &tmsTotalStart, &tmsTotalEnd);

    return 0;
}