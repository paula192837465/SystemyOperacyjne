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

#include "commands.h"

int queueServer = -1;
int clientsID[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int clientsQueue[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int occupied[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
int counter = 0;
key_t serverQueueKey = -1;
int active =1;
int numOfClients =0;

void delete_queue(){
    if (queueServer> -1){
        int tmp = msgctl(queueServer, IPC_RMID, NULL);
        if (tmp == -1){
            printf("error on deleting queue");
        }
        printf("deleted servers queue successfully");
    }
}

void log_in(MyMsg * msg){
    numOfClients++;
    clientsID[counter] = numOfClients;
    occupied[counter] = 0;
    clientsQueue[counter]=msg->clientQueueID;

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
    if (msgsnd(msg->clientQueueID, msg, MSG_SIZE, 0) == -1)   
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


    for(int i=0; i<10;i++)
    {
        //sprawdzić
         if(clientsID[i]==msg->friendID and occupied[i]==0)
         {
             //dorobic prawdzanie czy juz zajety
            printf("Jestem tu\n");
            fflush(stdout);
            MyMsg to_send_friend;
            to_send_friend.friendQueueID=msg->clientQueueID;
            to_send_friend.mtype = CONNECT;
            if(msgsnd(clientsQueue[i], &to_send_friend, MSG_SIZE,0)<0)
                perror("Error while sending info to friend");

            MyMsg to_send_client;
            to_send_client.friendQueueID=clientsQueue[i];
            to_send_client.mtype = CONNECT;

            if(msgsnd(msg->clientQueueID, &to_send_client, MSG_SIZE,0)<0)
                perror("Error while sending info to requesting client");
            occupied[i]=1;
         }
         if(clientsID[i]==msg->clientID)
            occupied[i]=1;
    }
   
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
    
    char * path =getenv("HOME");
    serverQueueKey = ftok(path, PROJECT_ID);

    queueServer = msgget(serverQueueKey, IPC_CREAT  | 0666);
    if(queueServer<0)
        perror("Lack of server queue");
    printf("Server queue: %d", queueServer);

    MyMsg msg;
    while(1)
     {
        
        if(msgrcv(queueServer, &msg, MSG_SIZE, 0, 0) < 0)
            perror("Error spra");
        
        processCommand(&msg);

     }

    return 0;

}