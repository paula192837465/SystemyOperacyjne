#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
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

    int res= open("pomiar_zad_3b.txt", O_RDWR | O_CREAT);
    char buffer [50];
    int n;
    n=sprintf (buffer, "\tREAL_TIME: %fl\n", timeDifference(start,end));
    write(res,buffer, sizeof(char)*n);
    n=sprintf (buffer, "\tUSER_TIME: %fl\n", timeDifference(t_start->tms_utime, t_end->tms_utime));
    write(res,buffer, sizeof(char)*n);
    n=sprintf (buffer, "\tSYSTEM_TIME: %fl\n",timeDifference(t_start->tms_stime, t_end->tms_stime));
    write(res,buffer, sizeof(char)*n);

    close(res);

}

int getLine(int file, int resA, int resB, int resC)
{
    char *letter = calloc(1, sizeof(char));
    int counter =0, readed;
    char * line = calloc(1, sizeof(char));

    while(readed= read(file, letter, sizeof(char))==1)
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

    
    if(atoi(line)%2==0)
    {
        strcat(line,letter);
        write(resA, line, sizeof(char)* strlen(line));
    }
    if(strlen(line)>2 && (atoi(line)/10%10==7 || atoi(line)/10%10==0))
    {
        strcat(line,letter);
        write(resB, line, sizeof(char)* strlen(line));
    }

    int temp= sqrt(atoi(line));
    if(temp*temp == atoi(line))
    {
        strcat(line, letter);
         write(resC, line, sizeof(char)* strlen(line));
    }
       
    free(letter);
    free(line);
    return 1;
}
void numbers(int A)
{
    int resA=open("a.txt", O_RDWR | O_CREAT);
    int resB=open("b.txt", O_RDWR | O_CREAT); 
    int resC=open("c.txt", O_RDWR | O_CREAT);

    int readed=0;

    while(readed!=-1)
    {
        readed = getLine(A, resA, resB, resC);
    }
    close(A);
    close(resA);
    close(resB);
    close(resC);
    
}
int main(int argc, char ** argv) {

    struct tms tmsTotalStart, tmsTotalEnd;
    clock_t totalStart, totalEnd;

    totalStart = times(&tmsTotalStart);

    if(argc<2)
        perror("To few arguments");

    int fileA = open(argv[1],O_RDONLY);
    
    if(fileA<0)
    {
        perror("There is no files");
        return 0;
    }
    numbers(fileA);

    totalEnd = times(&tmsTotalEnd);
    saveResults(totalStart, totalEnd, &tmsTotalStart, &tmsTotalEnd);

    return 0;
}