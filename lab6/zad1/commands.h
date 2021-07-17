#ifndef commands_h
#define commands_h

#define MAX_CLIENTS  10
#define PROJECT_ID 'p'
#define MAX_MSG_SIZE 4096
#define MAX_MSG_LEN 256
#define and &&

const char delims[3] = {' ', '\n', '\t'};

int error(char * msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

typedef enum mtype {
    LOGIN = 1, STOP = 2, LIST = 3, CONNECT =4, DISCONNECT =5, M=6
} mtype;

typedef struct MyMsg{
    long mtype;
    int clientID;
    int clientQueueID;
    int friendID;
    int friendQueueID;
    char mtext[MAX_MSG_SIZE];
} MyMsg;

const size_t MSG_SIZE = sizeof(MyMsg) - sizeof(long);

#endif