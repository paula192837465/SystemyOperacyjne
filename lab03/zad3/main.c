#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>


void searchFiles(char *dir, int depth, char* name )
{
    DIR *dp;
    struct dirent * file;
    struct stat statbuf;
    int len = strlen(name);

    if((dp = opendir(dir)) == NULL) 
    {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
    printf("\nCurrent directory: %s\n", dir);
    chdir(dir);
    while((file = readdir(dp)) != NULL) 
    {
        lstat(file->d_name,&statbuf);

        if(S_ISDIR(statbuf.st_mode)) 
        {
            if(strcmp(".",file->d_name) == 0 ||
                strcmp("..",file->d_name) == 0)
                continue;

            if(strncmp(file->d_name, name, len)==0)
                printf("%*s%s/\n",depth,"",file->d_name);

            pid_t child_pid;
            child_pid = fork();
            if(child_pid==0) 
            {
                printf("Proces dziecka: Proces rodzica ma pid:%d\n",(int)getppid());
                printf("Proces dziecka: Proces dziecka ma pid:%d\n",(int)getpid());
                
                searchFiles(file->d_name,depth+4, name);
                exit(0);
            }
        }
        else 
        {
            if(strncmp(file->d_name, name, len)==0)
                printf("%*s%s\n",depth,"",file->d_name);
        }

    }
    while(wait(NULL)>0);
    chdir("..");
    closedir(dp);
}

int main(int argc, char ** argv)
{
    if(argc<3)
    {
        perror("Too few arguments");
    }

    printf("PID glownego programu: %d\n", (int)getpid());
    searchFiles( argv[1],0,argv[2]);
    
    return 0;
}