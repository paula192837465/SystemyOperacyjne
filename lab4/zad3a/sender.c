#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int received = 0;

void handle_KILL(int sig, siginfo_t *info, void *ucontext) 
{
    if (sig == SIGUSR1) 
    {
        received++;
    } 
    else 
    {
        printf("Sender received: %d signals\n", received);
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
        printf("Sender reveived: %d signals\n", received);
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
        printf("Sender received: %d signals\n", received);
        exit(0);
    }
}

void sendSignals(char *mode, int numOfSignals, int catcherPID) 
{
    if (strcmp(mode, "KILL") == 0) 
    {
        for (int i = 0; i < numOfSignals; i++)
            kill( catcherPID, SIGUSR1);
        kill( catcherPID, SIGUSR2);
    } 
    else if (strcmp(mode, "SIGQUEUE") == 0) 
    {
        union sigval sigv;
        for (int i = 0; i < numOfSignals; i++)
            sigqueue(catcherPID, SIGUSR1, sigv);
        sigqueue(catcherPID, SIGUSR2, sigv);
    } 
    else if (strcmp(mode, "SIGRT") == 0) 
    {
        for (int i = 0; i < numOfSignals; i++)
            kill( catcherPID, SIGRTMIN);
        kill( catcherPID, SIGRTMAX);
    } 
    else 
    {
        perror("There is no such a mode");
    }
}

int main(int argc, char **argv) 
{
    if (argc < 4)
        perror("Too few args");

    int numSignals = atoi(argv[1]);
    int catcherPID = atoi(argv[2]);
    char *mode = argv[3];

    if (numSignals <= 0)
        perror("Wrong num of signals");
    if (catcherPID <= 0)
        perror("Wrong num of catcher pid");

    //zablokowanie odberania wszystkich sygnalow z wyjatkiem dwoch
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

    //wysylanie sygnalow
    sendSignals(mode, numSignals, catcherPID);
    
    //przypinanie handlerow
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

    while (1);
}


