#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int received = 0;

void handle_KILL(int sig, siginfo_t *info, void *ucontext) 
{
    if (sig == SIGUSR1) 
    {
        received++;
    } 
    else 
    {
        for (int i = 0; i < received; i++)
            kill(info->si_pid, SIGUSR1);
        kill(info->si_pid, SIGUSR2);
        printf("Catcher received: %d\n signals", received);
        exit(0);
    }
}

void handle_SIGQUEUE(int sig, siginfo_t *info, void *ucontext) 
{
    if (sig == SIGUSR1) 
    {
        received++;
    } 
    else 
    {
        union sigval sigv;
        for (int i = 0; i < received; i++) 
        {
            sigv.sival_int = i;
            sigqueue(info->si_pid, SIGUSR1, sigv);
        }
        sigv.sival_int = received - 1;
        sigqueue(info->si_pid, SIGUSR2, sigv);
        printf("Catcher revived: %d signals\n", received);
        exit(0);
    }
}

void handle_SIGRT(int sig, siginfo_t *info, void *ucontext) 
{
    if (sig == SIGRTMIN) 
    {
        received++;
    } 
    else 
    {
        for (int i = 0; i < received; i++)
            kill(info->si_pid, SIGRTMIN);
        kill(info->si_pid, SIGRTMAX);

        printf("Catcher received: %d signals\n", received);
        exit(0);
    }
}

int main(int argc, char **argv) {
    
    if (argc < 2) {
        perror("Too few args");
    }
    
    printf("Catcher PID: %d\n", getpid());
    char *mode = argv[1];

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    if (strcmp(mode, "KILL") == 0) 
    {
        act.sa_sigaction = handle_KILL;
        sigaction(SIGUSR1, &act, NULL);
        sigaction(SIGUSR2, &act, NULL);
    } 
    else if (strcmp(mode, "SIGQUEUE") == 0) 
    {
        act.sa_sigaction = handle_SIGQUEUE;
        sigaction(SIGUSR1, &act, NULL);
        sigaction(SIGUSR2, &act, NULL);
    } 
    else if (strcmp(mode, "SIGRT") == 0) 
    {
        act.sa_sigaction = handle_SIGRT;
        sigaction(SIGRTMIN, &act, NULL);
        sigaction(SIGRTMAX, &act, NULL);
    } 
    else 
    {
        perror("There is no such a mode");
    }
    
    sigset_t blockAll;
    sigfillset(&blockAll);
    if (strcmp(mode, "KILL") == 0) 
    {
        sigdelset(&blockAll, SIGUSR1);
        sigdelset(&blockAll, SIGUSR2);
    } 
    else if (strcmp(mode, "SIGQUEUE") == 0) 
    {
        sigdelset(&blockAll, SIGUSR1);
        sigdelset(&blockAll, SIGUSR2);
    } 
    else if (strcmp(mode, "SIGRT") == 0) 
    {
        sigdelset(&blockAll, SIGRTMIN);
        sigdelset(&blockAll, SIGRTMAX);
    } 
    else 
    {
        perror("There is no such a mode");
    }

    sigprocmask(SIG_BLOCK, &blockAll, NULL);

    while (1);
}

