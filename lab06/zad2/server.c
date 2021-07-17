#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <mqueue.h>

#include "commands.h"

int queueServer = -1;
int clientsID[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int clientsQueue[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int occupied[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
char pathClients[10][20];
int counter = 0;
key_t serverQueueKey = -1;
int active =1;
int numOfClients =0;
unsigned int priority=1;
char * path ="/serverQueue";

void delete_queue(){
    printf("ZAKONCZENIE");
    if (queueServer> -1){
        mq_close(queueServer);
        int tmp = mq_unlink(path);
        if (tmp == -1){
            printf("error on deleting queue");
        }
        
        for(int i=0;i<10;i++)
        {
            if(clientsID[i]>0)
            {
                MyMsg msg;
                msg.mtype = STOP;
                msg.clientQueueID = clientsQueue[i];
                if(mq_send(clientsQueue[i], &msg, MSG_SIZE,priority)<0)
                    perror("Error while closing queues");
                
            }
        }
        printf("deleted servers queue successfully");
    }
}

void log_in(MyMsg * msg){
    numOfClients++;
    clientsID[counter] = numOfClients;
    occupied[counter] = 0;
    clientsQueue[counter]=mq_open(msg->mtext, O_WRONLY);
    sprintf(pathClients[counter], "%s", msg->mtext);
    //printf("\nSCIEZKA: %s %d\n", msg->mtext, clientsQueue[counter]);
    int i=counter;

    printf("KOLEJKI: %d \n", clientsQueue[counter]);


    if (counter >= MAX_CLIENTS){
        printf("Maximum number of clients exceeded");
        sprintf(msg->mtext, "%d", -1);
    }
    else{
        sprintf(msg->mtext, "%d", counter);
        msg->clientID = numOfClients;
        while (clientsID[counter] > 0 && counter < MAX_CLIENTS) 
            counter++;
    }
    if (mq_send(clientsQueue[i], msg, MSG_SIZE, priority) == -1)   
        perror("login response failed");
}

void handle_sigint(int signum){
    printf("Odebrano sygnal SIGINT\n");
    exit(EXIT_SUCCESS);
}

void connect(MyMsg *msg)
{
    fflush(stdout);
    for(int i=0;i<10;i++)
    {printf("%d ", clientsID[i]);
    }
    printf("\n %d", msg->friendID);
    fflush(stdout);
    int pCLIENT, pFRIEND;
    MyMsg to_send_friend;
    MyMsg to_send_client;

    for(int i=0; i<10;i++)
    {
        
        //sprawdzić
         if(clientsID[i]==msg->friendID and occupied[i]==0)
         {
             //dorobic prawdzanie czy juz zajety
            printf("Jestem tu\n");
            fflush(stdout);
            
            to_send_friend.friendQueueID=msg->clientQueueID;
            to_send_friend.mtype = CONNECT;
            pFRIEND = i;
            
            occupied[i]=1;
         }
        if(clientsID[i]==msg->clientID and occupied[i]==0)
        {
           
            to_send_client.friendQueueID=clientsQueue[i];
            to_send_client.mtype = CONNECT;
            
            pCLIENT = i;
            occupied[i]=1;
         }
            
    }

    sprintf(to_send_friend.mtext, "%s", pathClients[pCLIENT]); 

    if(mq_send(clientsQueue[pFRIEND], &to_send_friend, MSG_SIZE,priority)<0)
        perror("Error while sending info to friend");
   
    sprintf(to_send_client.mtext, "%s",pathClients[pFRIEND]);
    if(mq_send(clientsQueue[pCLIENT], &to_send_client, MSG_SIZE,priority)<0)
        perror("Error while sending info to requesting client");
}

void disconnect(MyMsg *msg)
{
    for(int i=0; i<10;i++)
    {
        if(clientsID[i]==msg->friendID)
        {
            if(clientsQueue[i]==msg->clientID || clientsQueue[i]==msg->friendQueueID)
                occupied[i]=0;
        }
    }
}

void stop(MyMsg * msg){
    int i;
    for (i = 0; i < MAX_CLIENTS; ++i){
        if (clientsID[i] == msg->clientID){
            clientsID[i] = -1;
            clientsQueue[i] =-1;
            counter = i;
            occupied[i] =-1;
            
        }
    }
    printf("Client %d exited.\n", msg->clientID);
}

void list(){
    int i;
    printf("Active clients:\n");
    for (i = 0; i < MAX_CLIENTS; ++i){
        if (clientsID[i] > 0)
            printf(" -\t %d\n", clientsID[i]);
    }
}

void processCommand(MyMsg* msg){
    if (!msg) 
        return;

    switch(msg->mtype){
        case LOGIN:
            log_in(msg);
            printf("Logged user with client ID %d.\n", msg->clientID);
            break;
        case STOP:
            stop(msg);
            printf("Client %d is gone.\n", msg->clientID);
            break;
        case LIST:
            list();
            break;
        case CONNECT:
            connect(msg);
            break;
        case DISCONNECT:
            disconnect(msg);
            break;
    }
}

int main(int argc, char ** argv)
{
    if ( atexit(delete_queue) < 0)                        
        perror("atexit()");
    if ( signal(SIGINT, handle_sigint) == SIG_ERR)        
        perror("signal()");
    
    
    struct mq_attr serverQueue = {.mq_flags =0, .mq_maxmsg= 7,.mq_msgsize= MSG_SIZE, .mq_curmsgs= 0};

    int queueServer = mq_open(path, O_CREAT | O_RDONLY, 0666, &serverQueue);

    if(queueServer<0)
        perror("Lack of server queue");

    printf("Server queue: %d", queueServer);

    MyMsg msg;
    while(1)
    {
        
        if(mq_receive(queueServer, (char*) &msg, 4096, &priority) < 0)
            perror("Error spra");
        
        processCommand(&msg);

    }

    return 0;

}