#ifndef MyLibrary_h
#define MyLibrary_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Block
{
    int index;
    char **oneLine;
};

struct MainArray
{
    int index;
    struct Block **arrOfLines;
};

struct MainArray* createMainArray(int size);
struct Block* createSubArray(int size);
int combine2FILES(FILE *A, FILE *B);
char** splitFiles(char *combined);
int asignBlock(struct MainArray* mainArr, int size);
int combineFILES(char **args, int lenght, struct MainArray* mainArr);
void printfMainArr(struct MainArray* mainArray);
int numberOfLines(struct MainArray *arr, int index);
void deleteSubArray(int index, struct MainArray *arr);
void deleteLineFromSubArr(int MainIndex, int Subindex, struct MainArray* arr);

double timeDifference(clock_t t1, clock_t t2);

#endif