#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

void createProcess(int num)
{
    pid_t child_pid;
    printf("PID glownego programu: %d\n", (int)getpid());
    pid_t parent =(int)getpid();
    int i;
    for(i=0; i<num;i++)
    {
        if(parent== getpid())
        {
          
            child_pid = fork();
            if(child_pid!=0) {
            continue;
            } 
            else 
            {
                printf("\n\njestem procesem nr %d\n\n", i+1);
                printf("Proces dziecka: Proces rodzica ma pid:%d\n",(int)getppid());
                printf("Proces dziecka: Proces dziecka ma pid:%d\n",(int)getpid());
                exit(0);
            }
        }
    } 
    if(parent==getpid()) 
            while(wait(NULL)>0);
   
}

int main(int argc, char ** argv) 
{
    if(argc<2)
    {
        perror("Too few arguments");
    }
    createProcess(atoi(argv[1]));
    return 0;
}