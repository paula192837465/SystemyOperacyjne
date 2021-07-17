#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "MyLibrary.h"
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/times.h>
#include <dlfcn.h>
#include <ctype.h>
#include <time.h>


int main(int argc, char ** argv) 
{
    if(argc<2)
    {
        perror("Wrong command!");
        exit(1);
    }

    struct MainArray* mainArray;
    int k;
    int argslen;
    printf("PID glownego programu: %d\n", (int)getpid());

    for(k=0;k<argc;k++)
    {
        if(strcmp(argv[k], "create_table")==0)
        {
            if(argv[k+1]==NULL)
                perror("Nie podałes rozmiaru tablicy!");
            mainArray = createMainArray(atoi(argv[k+1]));
        }
        else if (strcmp(argv[k],"merge_files")==0)
        {
            int i, j,ifFile, end=0; 
            int argslen=0;
            char **stringsArray = calloc(argc-1, sizeof(char*));

            for(i=k+1;i<argc;i++)
            {
                ifFile=0;
                j=0;
                while(argv[i][j]!='\000')
                {
                    if(argv[i][j]==':')
                        ifFile=1;
                    if(argv[i][j]=='_') //zakladamy ze w nazwie pliku nie moze by podlogi
                        end=1;
                    j++;
                }
                if(end==1)
                    break;

                if(ifFile==1)
                {
                    stringsArray[argslen]=calloc(j+1, sizeof (char));
                    strcpy(stringsArray[argslen], argv[i]); 
                    argslen++;
                }
            }
            combineFILES(stringsArray,argslen,mainArray);
            free(stringsArray);

        }
        else if(strcmp(argv[k], "remove_block")==0)
        {
            deleteSubArray(atoi(argv[k+1]),mainArray);
        }
        else if(strcmp(argv[k], "remove_row")==0)
        {
            deleteLineFromSubArr(atoi(argv[k+1]), atoi(argv[k+2]),mainArray);
        }
    }

    printfMainArr(mainArray);
    printf("%d", mainArray->index);
}