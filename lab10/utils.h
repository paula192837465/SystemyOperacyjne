#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>

#define PORT 8082
#define MAX_CLIENTS 10
#define MSG_LEN 1024


struct Client {
    char name[32];
    int socket;
    int is_alive;
    int opponentSocket; 
};