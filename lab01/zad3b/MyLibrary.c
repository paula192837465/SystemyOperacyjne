#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MyLibrary.h"
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

void writeResult(clock_t start, clock_t end, struct tms* t_start, struct tms* t_end)
{
   printf("\tREAL_TIME: %fl\n", timeDifference(start,end));
    printf("\tUSER_TIME: %fl\n", timeDifference(t_start->tms_utime, t_end->tms_utime));
    printf("\tSYSTEM_TIME: %fl\n", timeDifference(t_start->tms_stime, t_end->tms_stime));

    // FILE* res= fopen("results.txt", "a+");
    // char buffer [50];
    // int n;
    // n=sprintf (buffer, "\n\n\n\t%s :\n", description);
    // fwrite(buffer,sizeof(char),n, res);
    // n=sprintf (buffer, "\tREAL_TIME: %fl\n", timeDifference(start,end));
    // fwrite(buffer,sizeof(char),n, res);
    // n=sprintf (buffer, "\tUSER_TIME: %fl\n", timeDifference(t_start->tms_utime, t_end->tms_utime));
    // fwrite(buffer,sizeof(char),n, res);
    // n=sprintf (buffer, "\tSYSTEM_TIME: %fl\n",timeDifference(t_start->tms_stime, t_end->tms_stime));
    // fwrite(buffer,sizeof(char),n, res);

    // fclose(res);  
    // printf("\tREAL_TIME: %fl\n", timeDifference(start,end));
    // printf("\tUSER_TIME: %fl\n", timeDifference(t_start->tms_utime, t_end->tms_utime));
    // printf("\tSYSTEM_TIME: %fl\n", timeDifference(t_start->tms_stime, t_end->tms_stime));
}

struct MainArray* createMainArray(int size)
{
    struct MainArray* arr=(struct MainArray*) calloc(1, sizeof(struct MainArray));

    arr->arrOfLines = (struct Block*) calloc(size, sizeof (struct Block*));
    arr->index = -1;

    return arr;
}

struct Block* createSubArray(int size)
{
    struct Block* arr = (struct Block*)calloc(1, sizeof(struct Block*));

    arr->index=-1;
    arr->oneLine = calloc(size, sizeof (char *));

    return arr;
}

int combine2FILES(FILE *A, FILE *B) 
{

    char line[300];
    int sizeOfFile=0;
    FILE *generated = fopen("merge.txt", "w"); //tworzenie pliku tymczasowego

    fseek(A,0,0);
    fseek(B, 0,0);

    while(!feof(A) && !feof(B))
    {
        fgets(line,sizeof (line), A);
        fwrite(line, sizeof(char), strlen(line), generated);
        fgets(line, sizeof (line), B);
        fwrite(line, sizeof(char), strlen(line), generated);
        sizeOfFile+=2;
    }

    while(!feof(B))
    {
        fgets(line, sizeof (line), B);
        fwrite(line, sizeof(char), strlen(line), generated);
        sizeOfFile++;
    }
    while(!feof(A))
    {
        fgets(line, sizeof (line), A);
        fwrite(line, sizeof(char), strlen(line), generated);
        sizeOfFile++;
    }
    fclose(A);
    fclose(B);
    fclose(generated);

    return sizeOfFile;
}

char** splitFiles(char *combined)
{

    char *copiedArr =(char*)calloc(strlen(combined)+1, sizeof(char));
    strcpy(copiedArr,combined);

    char **result;
    result = calloc(2, sizeof(char*));

    int i=0;
    while(combined[i]!=':')
        i++;

    copiedArr[i]='\0';
    result[0]=&(copiedArr[0]);
    result[1]=&(copiedArr[i+1]);

    return result;
}

void printfMainArr(struct MainArray* mainArray)
{
    int i,j;
    for(i=0;i<mainArray->index+1;i++)
    {
        for (j=0;j<mainArray->arrOfLines[i]->index;j++)
        {
            printf("%s\n",mainArray->arrOfLines[i]->oneLine[j]);
        }
        printf("---------------------------------");
    }
}

int asignBlock(struct MainArray* mainArr, int size)
{
    char line[300];
    int j;
    FILE *temp = fopen("merge.txt","r");

    if(temp==NULL)
        printf("Cannot open a new file ;(");

    struct Block* newBlock = createSubArray(size);

    
    for(j=0;j<size;j++)
    {
        fgets(line, sizeof(line), temp);
        newBlock->oneLine[j] = calloc(strlen(line)+1, sizeof (char));
        newBlock->index++;
        strcpy(newBlock->oneLine[j],line);
    }
    fclose(temp);

    mainArr->index++;
    mainArr->arrOfLines[mainArr->index] = newBlock;

    return mainArr->index;
}

int combineFILES(char **args, int lenght, struct MainArray* mainArr)
{
    struct tms tmsStart, tmsEnd;
    clock_t start, end;
    
    

    int i, size,j=0;
    FILE *fileA;
    FILE *fileB;
    char **files = calloc(2,sizeof(char*));

    for(i=0;i<lenght;i++)
    {
        if(args[i]==NULL)
            break;
        files=splitFiles(args[i]);

        fileA = fopen(files[0], "r");
        fileB = fopen(files[1], "r");

        if (fileA == NULL || fileB==NULL)
        {
            perror("Cannot open file :(");
            exit(1);
        }

        start = times(&tmsStart);
        size = combine2FILES(fileA, fileB);
        end = times(&tmsEnd);
        printf("\n\nCzas mergowania dwoch plikow: \n");
        writeResult(start, end, &tmsStart, &tmsEnd);

        start = times(&tmsStart);
        asignBlock(mainArr, size);
        end = times(&tmsEnd);
        printf("\nCzas przypinania bloku do tablicy glownej: \n");
        writeResult(start, end, &tmsStart, &tmsEnd);
    }
}

int numberOfLines(struct MainArray *arr, int index)
{
    return arr->arrOfLines[index]->index;
}

void deleteSubArray(int index, struct MainArray* arr)
{
    int i;
    free(arr->arrOfLines[index]->oneLine);
    for(i=index;i<arr->index-1;i++)
        arr->arrOfLines[i]= arr->arrOfLines[i+1];
    arr->index--;
}

void deleteLineFromSubArr(int MainIndex, int Subindex, struct MainArray* arr)
{
    int i;
    free(arr->arrOfLines[MainIndex]->oneLine[Subindex]);
    for(i=Subindex;i<arr->arrOfLines[MainIndex]->index-1;i++)
        arr->arrOfLines[MainIndex]->oneLine[i]=arr->arrOfLines[MainIndex]->oneLine[i+1];
    arr->arrOfLines[MainIndex]->index--;
}