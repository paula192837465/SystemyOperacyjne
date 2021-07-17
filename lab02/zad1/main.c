#include <stdio.h>
#include <stdlib.h>
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

    FILE* res= fopen("pomiar_zad_1a.txt", "w+");
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

int getLine(FILE* file)
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
        
        line = realloc(line,  sizeof(char)* strlen(line)+2);
        strcat(line, letter);
        counter++;
    }
    if(readed==0)
        return -1;
    strcat(line,letter);
    printf("%s", line);
    
    free(letter);
    free(line);
    return 1;
}

void combine2Files(FILE * A, FILE* B)
{
    int startA, startB;

    startA=0;
    startB=0;
    int readed=0;

    while(readed!=-1 && readed!=-1)
    {
        readed=getLine(A);
        readed=getLine(B);
    }
    readed=0;
    while(readed!=-1)
        readed=getLine(A);
    readed=0;
    while(readed!=-1)
        readed=getLine(B);

    fclose(A);
    fclose(B);

}
int main(int argc, char ** argv) {

    struct tms tmsTotalStart, tmsTotalEnd;
    clock_t totalStart, totalEnd;

    totalStart = times(&tmsTotalStart);

    if(argc<2)
        perror("To few arguments");

    FILE* fileA = fopen(argv[1],"r");
    FILE* fileB =fopen(argv[2], "r");
    if(fileA==NULL || fileB==NULL)
    {
        perror("There is no files");
        return 0;
    }
    combine2Files(fileA, fileB);

    totalEnd = times(&tmsTotalEnd);
    saveResults(totalStart, totalEnd, &tmsTotalStart, &tmsTotalEnd);

    return 0;
}
