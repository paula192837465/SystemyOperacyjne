#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>


void my_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        printf("Received SIGUSR1!\n");
    }
}

int main(int argc, char **argv)
{

    printf("%d\n", getpid());
    if(argc<2)
    {
        perror("Za malo argumentow");
    }

    if(strcmp(argv[1],"ignore")==0)
    {
        signal(SIGUSR1, SIG_IGN);
        raise(SIGUSR1);
        pid_t child;
        if(child=fork()==-1)
        {
            printf("nie udalo sie stworzyc procesu potomnego");
            exit(0);
        }
        else if(child==0)
        {
            printf("%d\n", getpid());
            raise(SIGUSR1);
        }
        
    }
    else if(strcmp(argv[1],"handler")==0)
    {
        signal(SIGUSR1, my_handler);
        raise(SIGUSR1);
        pid_t child;
        if(child=fork()==-1)
        {
            printf("nie udalo sie stworzyc procesu potomnego");
            exit(0);
        }
        else if(child==0)
        {
            printf("%d\n", getpid());
            raise(SIGUSR1);
        }
    }

    else if(strcmp(argv[1],"mask")==0)
    {
        sigset_t newMask;
        sigset_t oldMask;
        

        sigemptyset(&newMask);
        sigaddset(&newMask, SIGUSR1);
        int block;
        if(block = sigprocmask(SIG_BLOCK, &newMask, &oldMask)<0)
            perror("Nie udalo sie zablokowac sygnalu");


        printf("%d\n", sigismember(&newMask, SIGUSR1));
        raise(SIGUSR1);

        pid_t child;
        if(child=fork()==-1)
        {
            printf("nie udalo sie stworzyc procesu potomnego");
            exit(0);
        }
        else if(child==0)
        {
            printf("%d\n", getpid());
            raise(SIGUSR1);
            printf("%d\n", sigismember(&newMask, SIGUSR1));
        }

    }
    else if(strcmp(argv[1],"pending")==0)
    {
      
        sigset_t newMask;
        sigset_t oldMask;
        
        sigemptyset(&newMask);
        sigaddset(&newMask, SIGUSR1);
        int block;
        
        if(block = sigprocmask(SIG_BLOCK, &newMask, &oldMask)<0)
            perror("Nie udalo sie zablokowac sygnalu");
     
        raise(SIGUSR1);
        sigset_t curr;
        sigpending(&curr);
        printf("%d\n", sigismember(&curr, SIGUSR1));
        printf("%d\n", sigismember(&curr, SIGUSR2));

        pid_t child;
        if(child=fork()==-1)
        {
            printf("nie udalo sie stworzyc procesu potomnego");
            exit(0);
        }
        else if(child==0)
        {
            printf("%d\n", getpid());
            sigpending(&curr);
            printf("%d\n", sigismember(&curr, SIGUSR1));
            printf("%d\n", sigismember(&curr, SIGUSR2));
            
        }
    }
    
}
