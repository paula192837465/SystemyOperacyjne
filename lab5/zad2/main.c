#define _POSIX_C_SOURCE 2
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


int main(int argc, char **argv)
{
    char *cmd;

    FILE* file; 
    if(argc == 2)
    {

        if(strcmp(argv[1], "data")==0)
        {
            file = popen("echo | mail | tail -n +2 | head -n -2 | tac", "r");
            
        }
        else if(strcmp(argv[1], "nadawca")==0)
        {
            file = popen("echo | mail | tail -n +2 | head -n -2 | sort -u -k3", "r");
        }

        char buffer[257];
        while (fgets(buffer, sizeof(buffer) - 1, file) != NULL) 
        {
            printf("%s", buffer);
        }
    }
    else if(argc==4)
    {
        cmd=calloc(strlen(argv[1])+strlen(argv[2]) + strlen(argv[3])+15, sizeof(char));
        sprintf(cmd, "echo %s | mail -s %s %s", argv[3], argv[2], argv[1]);

        file = popen(cmd,"w"); 
        free(cmd);  
    }
   

    fclose(file);

}