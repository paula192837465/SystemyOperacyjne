#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>


#define exec(prog, args...) \
        if(fork()==0) execl(prog, prog, args, NULL)


int main(int argc, char **argv)
{
    char transfer[] = "transfer";
    mkfifo(transfer, 0666); 
    exec("./consument", transfer, "result.txt", "6", NULL);
    exec("./producent", transfer, "1", "text1.txt", "4", NULL);
    exec("./producent", transfer, "2", "text2.txt", "4", NULL);
    //exec("./producent", transfer, "4", "text3.txt", "4", NULL);


    while (wait(NULL)>0);
    remove(transfer);
    return 0;
}
