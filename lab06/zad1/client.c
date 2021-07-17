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

#include "commands.h"

int serverQueueID =-1;
int clientQueueID =-1;

int clientID =-1;
int counter=0;
int friendQueue =-1;

void connect(char* string)
{
    int friendID = atoi(string);
    printf("Connect test: %d", friendID);
    MyMsg msg;
    msg.mtype = CONNECT;
    msg.clientID = clientID;
    msg.clientQueueID = clientQueueID;
    msg.friendID=friendID;
    if (msgsnd(serverQueueID, &msg, MSG_SIZE, 0) < 0)      
        perror("server friends msgsnd failed");
    
}

void receivedConnect(MyMsg* received)
{
    printf("Connected to %d", received->friendQueueID);
    friendQueue = received->friendQueueID;
}

void disconnect()
{
    MyMsg msg;
    msg.mtype = DISCONNECT;
    msg.clientID = clientID;
    msg.clientQueueID = clientQueueID;
    msg.friendQueueID = friendQueue;
    if (msgsnd(friendQueue, &msg, MSG_SIZE, 0) < 0)      
        perror("sending disconnect failed");
    if (msgsnd(serverQueueID, &msg, MSG_SIZE, 0) < 0)      
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
    if (msgsnd(friendQueue, &msg, MSG_SIZE, 0) < 0)      
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
    if (msgsnd(serverQueueID, &msg, MSG_SIZE, 0) < 0)      
        perror("client echo msgsnd failed");
    exit(0);
}

void list(){
    MyMsg msg;
    msg.clientID = clientID;
    msg.mtype = LIST;
    msg.clientQueueID = clientQueueID;
    sprintf(msg.mtext, "%s", "List");
    if (msgsnd(serverQueueID, &msg, MSG_SIZE, 0) < 0)      
        perror("client list msgsnd failed");
}


void delete_queue(){
    if ( msgctl(clientQueueID, IPC_RMID, NULL) < 0){
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
    // rest = strtok_r(rest, "\n", &rest); // ucinam enter na koncu
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

int open_queue(char * path, char ID){
    int key = ftok(path, ID);
    if (key == -1)    
        perror("ftok in create_queue");
    int QueueID = msgget(key, 0);
    if (QueueID < 0)     
        perror("msgget in create_queue");

    return QueueID;
}

void register_client(){
    MyMsg msg;
    msg.mtype = LOGIN;
    msg.clientQueueID = clientQueueID;
    MyMsg received;

    sprintf(msg.mtext, "%s", "Login");

    if ( msgsnd(serverQueueID, &msg, MSG_SIZE, 0) < 0)      
        perror("client login failed");
    if ( msgrcv(clientQueueID, &received, MSG_SIZE, LOGIN, 0) < 0)     
        perror("server login response failed");
    clientID =received.clientID; 
}

int main(int argc, char ** argv)
{
    if ( atexit(delete_queue) < 0)                  
        perror("atexit");
    if ( signal(SIGINT, handle_sigint) == SIG_ERR)  
        perror("signal");

    char *path = getenv("HOME");
    serverQueueID = open_queue(path, PROJECT_ID);
    key_t clientKey = ftok(path, getpid());

    clientQueueID = msgget(clientKey, IPC_CREAT | IPC_EXCL | 0666);
    if(clientQueueID<0)
        perror("There is no client queue");
    printf("Server queue2: %d", serverQueueID);
    
    register_client();

    char * buff;
    size_t len = 0;
    while(1){
        MyMsg received;
        if(msgrcv(clientQueueID, &received, MSG_SIZE, 0, IPC_NOWAIT)>=0)
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