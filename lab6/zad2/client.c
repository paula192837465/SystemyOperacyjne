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
#include <mqueue.h>
#include <time.h>
#include "commands.h"
unsigned int priority;

int serverQueueID =-1;
int clientQueueID =-1;

int clientID =-1;
int counter=0;
int friendQueue =-1;
char pathClient[20]; 
MyMsg received;

void connect(char* string)
{
    int friendID = atoi(string);
    printf("Connect test: %d", friendID);
    MyMsg msg;
    msg.mtype = CONNECT;
    msg.clientID = clientID;
    msg.clientQueueID = clientQueueID;
    msg.friendID=friendID;
    sprintf(msg.mtext, "%s", pathClient);
    if (mq_send(serverQueueID, &msg, MSG_SIZE, priority) < 0)      
        perror("server friends msgsnd failed");
    
}

void receivedConnect(MyMsg* received)
{
    friendQueue = mq_open(received->mtext, O_WRONLY);
    printf("Connected to %d", friendQueue);
}

void disconnect()
{
    MyMsg msg;
    msg.mtype = DISCONNECT;
    msg.clientID = clientID;
    msg.clientQueueID = clientQueueID;
    msg.friendQueueID = friendQueue;
    if (mq_send(friendQueue, &msg, MSG_SIZE, priority) < 0)      
        perror("sending disconnect failed");
    if (mq_send(serverQueueID, &msg, MSG_SIZE, priority) < 0)      
        perror("sending disconnect failed");
    friendQueue=-1;
}

void receivedDisconnect(MyMsg* msg)
{
    friendQueue=-1;
    printf("You are not a friend with %d anymore\n", msg->clientID);
}

void message()
{
    char * buff;
    size_t len = 0;
    getline(&buff, &len, stdin);
    printf("test3: %s\n", buff);
    
    MyMsg msg;
    msg.mtype = M;
    sprintf(msg.mtext, "%s", buff);
    if (mq_send(friendQueue, &msg, MSG_SIZE, priority) < 0)      
        perror("sending disconnect failed");

}

void receivedMedssage(MyMsg* msg)
{
    printf("Received a message from a friend: %s", msg->mtext);
}

void stop(){
    MyMsg msg;
    msg.clientID = clientID;
    msg.mtype = STOP;
    msg.clientQueueID=clientQueueID;
    sprintf(msg.mtext, "%s", "Stop");
    if (mq_send(serverQueueID, &msg, MSG_SIZE, priority) < 0)      
        perror("client echo msgsnd failed");
    exit(0);
}

void list(){
    MyMsg msg;
    msg.clientID = clientID;
    msg.mtype = LIST;
    msg.clientQueueID = clientQueueID;
    sprintf(msg.mtext, "%s", "List");
    if (mq_send(serverQueueID, &msg, MSG_SIZE, priority) < 0)      
        perror("client list msgsnd failed");
}


void delete_queue(){
    sprintf(pathClient, "/serverClient%d", getpid());
    mq_close(clientQueueID);
    if ( mq_unlink(pathClient) < 0){
        printf("main queue remove");
    }
    else{
        printf("Deleted successfully.\n");
    }
}

void handle_sigint(int signum){
    printf("Odebrano sygnal SIGINT\n");
    stop();
}

void processCommand(char * cmd, char * rest){
    if (strcmp(cmd, "LIST") == 0){
        list();
    }
    else if (strcmp(cmd, "STOP") == 0){
        stop();
    }
    else if (strcmp(cmd, "connect")==0)
    {
        connect(rest);
    }
    else if(strcmp(cmd, "DISCONNECT")==0)
    {
        disconnect(rest);
    }
    else if(strcmp(cmd, "M")==0 and friendQueue>0)
    {
        message();
    }
    else {
        printf("Bad command\n");
    }
}


void register_client(char * clientPath){
    MyMsg msg;
    msg.mtype = LOGIN;
    msg.clientQueueID = clientQueueID;
    sprintf(msg.mtext, "%s", clientPath);
    


    if ( mq_send(serverQueueID, &msg, MSG_SIZE, priority) < 0)      
        perror("client login failed");
    if ( mq_receive(clientQueueID, &received, MSG_SIZE, &priority) < 0)     
        perror("server login response failed");
    clientID =received.clientID; 
}

int main(int argc, char ** argv)
{
    if ( atexit(delete_queue) < 0)                  
        perror("atexit");
    if ( signal(SIGINT, handle_sigint) == SIG_ERR)  
        perror("signal");

  
    char * path ="/serverQueue";

     serverQueueID = mq_open(path, O_WRONLY);

    if(serverQueueID<0)
        perror("Lack of server queue");

    
    sprintf(pathClient, "/serverClient%d", getpid());
    struct mq_attr clientQueue = {0, 7, MSG_SIZE, 0};

    clientQueueID = mq_open(pathClient, O_CREAT | O_RDONLY, 0666, &clientQueue);

    printf("CLIENT QUEUE ID: %s %d %d\n",pathClient, getpid(), clientQueueID);
    if(clientQueueID<0)
        perror("There is no client queue");
    printf("Server queue2: %d", serverQueueID);
    
    register_client(pathClient);

    char * buff;
    size_t len = 0;

   
    while(1){
        // MyMsg received;
        struct timespec timee;
        clock_gettime(CLOCK_REALTIME, &timee);
        timee.tv_sec+=1;

        if(mq_timedreceive(clientQueueID, &received, MSG_SIZE, &priority, &timee)>=0)
        {
            switch(received.mtype){
            case STOP:
                stop(received);
                printf("Client %d is gone.\n", received.clientID);
                break;
            case CONNECT:
                receivedConnect(&received);
                break;
            case DISCONNECT:
                receivedDisconnect(&received);
                break;
            case M:
                receivedMedssage(&received);
                break;
                }
        }
          

        printf("\nType command:\n> ");
        getline(&buff, &len, stdin);
        char * rest;
        char * cmd = strtok_r(buff, delims, &rest);
       
        if (!cmd){
            printf("empty string\n");
            continue;
        }
        processCommand(cmd, rest);
    }

    return 0;

}