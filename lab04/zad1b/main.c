#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char **argv)
{

    printf("%d\n", getpid());
    if(argc<2)
    {
        perror("Za malo argumentow");
    }

    if(strcmp(argv[1],"ignore")==0)
    {
        if(!argv[2])
            signal(SIGUSR1, SIG_IGN);
        raise(SIGUSR1);
    }

    else if(strcmp(argv[1],"mask")==0)
    {
        sigset_t newMask;
        sigset_t oldMask;
        
        if(!argv[2])
        {
            sigemptyset(&newMask);
            sigaddset(&newMask, SIGUSR1);
            int block;
            if(block = sigprocmask(SIG_BLOCK, &newMask, &oldMask)<0)
                perror("Nie udalo sie zablokowac sygnalu");

        }
        raise(SIGUSR1);

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
     
        sigset_t curr;
        if(!argv[2])
            raise(SIGUSR1);
        sigpending(&curr);
        printf("%d\n", sigismember(&curr, SIGUSR1));
        printf("%d\n", sigismember(&curr, SIGUSR2));
    }

    //warunek sprawdzajacy czy funkcja exec juz sie wykonala
    if(!argv[2])
    {
       
        pid_t child;
        char *args[]={"./main", argv[1],"1", NULL};
        execvp(args[0], args);
    }

}
