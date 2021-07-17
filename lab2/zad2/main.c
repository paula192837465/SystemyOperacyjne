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

    FILE* res= fopen("pomiar_zad_2a.txt", "w+");
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

int getLine(FILE* file, FILE* resultFile, int position, char character)
{
    fseek(file, position, 0);
    char *letter = calloc(1,sizeof(char));
    int counter =0, withChar=0;
    fread(letter,sizeof (char), 1,file);

    if(letter[0]==NULL)
        return -1;

    while(letter[0] !='\n')
    {
        if(letter[0]==character)
            withChar = 1;
        fread(letter,sizeof (char), 1,file);;
        counter++;
    }
  
    fseek(file,position,0);
    char *result = calloc(counter+1, sizeof(char));
    fread(result,counter+1, sizeof (char), file);

    if(withChar==1)
        fwrite(result,sizeof(char),counter+1,resultFile);

    fgetpos(file,&position);

    free(result);
    free(letter);
    return position;
}
void searchFile(FILE * A, char character)
{
    int startA;
    FILE* resultFILE = fopen("result.txt", "w");
    startA=0;

    while(startA!=-1)
        startA=getLine(A, resultFILE, startA, character);

    fclose(A);
    fclose(resultFILE);
}

int main(int argc, char ** argv) {

    struct tms tmsTotalStart, tmsTotalEnd;
    clock_t totalStart, totalEnd;

    totalStart = times(&tmsTotalStart);

    if(argc<2)
        perror("To few arguments");

    FILE* fileA = fopen(argv[2],"r");
    char letter = argv[1][0];
    if(fileA==NULL)
    {
        perror("There is no such a file");
        return 0;
    }
    searchFile(fileA, letter);
    totalEnd = times(&tmsTotalEnd);
    saveResults(totalStart, totalEnd, &tmsTotalStart, &tmsTotalEnd);

    return 0;
}