#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_CMDS 5
#define DEF_LEN 70
#define DEF_MAX_COM 10
#define OUT 1
#define IN 0 

struct map
{
    char *command;
    char **line;
    int numOfPrograms;
};

char* copy(char* str)
{
    char* result =calloc(strlen(str)+1, sizeof(char));
    strncpy(result, str, strlen(str));
    return result;
}

char** splitCommand(char* line, int* numberOfPrograms)
{
    char** result = calloc(MAX_CMDS, sizeof(char*));
    int i=0, j;
    char* buffor;
    char delims[] ="|\n";
    buffor = strtok(line, delims);
    while(buffor !=NULL)
    {
        result[i] = copy(buffor);
        // printf("buffor: %s\n",buffor);
        buffor=strtok(NULL,delims);
        i++;
    }
    *numberOfPrograms=i;
    return result;
}

struct map* createDatabase(FILE* file)
{
    struct map* arr= calloc(DEF_MAX_COM, sizeof(struct map));
    char *buffor =NULL;
    size_t leng =0;
    int i=0;

    while(getline(&buffor,&leng,file)>1)
    {
        char delims[] = "=";
        char* clipboard;
        clipboard = strtok(buffor, delims);
        arr[i].command = copy(clipboard);

        clipboard=strtok(NULL,delims);
        int n;
        arr[i].line = splitCommand(clipboard, &n);
        arr[i].numOfPrograms = n;

        i++;
    }
    
    return arr;
}

int extractNumber(char * line)
{
    int num;
    if (1 == sscanf(line, "%*[^0123456789]%d", &num))
    {
        return num;
    }
}

void firstProcess( int fd[2][2])
{
    close(fd[OUT][IN]);
    dup2(fd[OUT][OUT], STDOUT_FILENO);
}

void middleProcess(int fd[2][2])
{
    close(fd[IN][OUT]);
    close(fd[OUT][IN]);
    dup2(fd[IN][IN], STDIN_FILENO);
    dup2(fd[OUT][OUT], STDOUT_FILENO);
}

void endProcess(int fd[2][2])
{
    close(fd[IN][OUT]);
    close(fd[OUT][OUT]);
    close(fd[OUT][IN]);
    dup2(fd[IN][IN], STDIN_FILENO);
}

void swapPipes(int fd[2][2])
{
    close(fd[IN][OUT]);
    fd[IN][IN] = fd[OUT][IN];
    fd[IN][OUT] = fd[OUT][OUT];
    pipe(fd[OUT]); 
}

void createProcess(char* prog, char**args, int last, int first)
{
    int fd[2][2];
    pipe(fd[OUT]);
    int i;
    
    if(fork()==0)
    {
        if(last==1)
            endProcess(fd);
        else if(first==1)
            firstProcess(fd);
        else
            middleProcess(fd);  
        
        for(i=0;i<MAX_CMDS;i++)
            printf("%s ", args[i]);
        printf("PROG %s LAST %d FIRST %d\n", prog,last, first);

         execvp(prog, args);
    }
    else
    {
        swapPipes(fd);
        wait(NULL);
    }

}

void startCommand(char* command, struct map* arr, int last, int first)
{
    char* prog;
    char ** args = calloc(MAX_CMDS, sizeof(char*));

    char delims[] = " ";
    char* clipboard;
    clipboard = strtok(command, delims);

    char* element;
    int lastEl = arr[extractNumber(clipboard)].numOfPrograms;
    int firstOfAll =first;
    for(int k =0;k<lastEl;k++)
    {
        element = strtok(arr[extractNumber(clipboard)].line[k], delims);
    
        prog = element;
        int i=0,j;

        while(element!=NULL)
        {
            args[i]= element;
            element = strtok(NULL, delims);
            i++;
        }
        args[i] = NULL;

        if(last==1 && lastEl==k+1)
            createProcess(prog, args,1, firstOfAll); 
        else
            createProcess(prog, args,0, firstOfAll);

        firstOfAll=0; 
    }
    

}

void readComponents(FILE* file, struct map* arr)
{
    char *buffor =NULL;
    size_t leng =0;
    int i=0;

    while(getline(&buffor,&leng,file)>1)
    {
        char delims[] = "| ";
        char* clipboard;
        char *rest = buffor;
        clipboard = strtok_r(buffor, delims , &rest);
        int first=1;
        while(clipboard!=NULL)
        {
            char* cos = copy(clipboard);
            clipboard = strtok_r(NULL, delims , &rest);

            if(clipboard==NULL)
                startCommand(cos, arr, 1, first);
            else
                startCommand(cos, arr, 0, first);
            
            first=0;

        }
    }
}

int main(int argc, char **argv)
{
     if (argc  != 2){
        perror("Bad args number");
    }

    FILE * file;
    
    char * line = calloc (70,sizeof(char));
    
    file = fopen(argv[1], "r");
    struct map* arr;

    if (file == NULL)
        perror("fopen");

    arr= createDatabase(file);

    readComponents(file, arr);

}