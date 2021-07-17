#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

void info_handler(int signo, siginfo_t *info, void *uncontext)
{
    //Dla sygnalu SIGINT info=>si_pid zawsze zwraca 0
    printf("Number of signal %d\nSending PID: %ld\nSignal code: %d\nReal user ID: %d\n\n", info->si_signo, (long)info->si_pid, info->si_code, info->si_uid);
}

void child_handler(int signo)
{
    printf("Otrzymano SIGCHLD. Numer sygnau: %d\n", signo);
}

int main(int argc, char **argv)
{
    printf("Pid: %d\n", getpid());
    struct sigaction act;
    act.sa_sigaction = &info_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);
    raise(SIGUSR1);

    //nie wyswietla informacji o tym ze proces dziecka sie zatrzymal/skonczyl
    act.sa_handler = &child_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &act, NULL);

    pid_t child1 = fork();
    if(child1==0)
    {
        raise(SIGSTOP);
    }

    //odsluguje tylko pierwszy sygnal
    act.sa_handler = &child_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_ONESHOT;
    sigaction(SIGCHLD, &act, NULL);

    pid_t child2 = fork();
    if(child2==0)
    {
        raise(SIGSTOP);
    }
    sleep(1);

    pid_t child3 = fork();
    if(child3==0)
    {
        raise(SIGSTOP);
    }
    sleep(1);

    pid_t child4 = fork();
    if(child4==0)
    {
        raise(SIGSTOP);
    }
    sleep(1);

    kill(child1, SIGKILL);
    kill(child2, SIGKILL);
    kill(child3, SIGKILL);
    kill(child4, SIGKILL);
}
