#include "utils.h"

int local_socket=0;
int network_socket=0;
int number_of_clients;
struct Client clients[MAX_CLIENTS];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void init_local(char *path)
{
     if ((local_socket = socket(AF_UNIX, SOCK_STREAM, 0)) <= 0) {
        perror("init of local socket failed");
        exit(1);
    }

    struct sockaddr_un sa;
    sa.sun_family = AF_UNIX;
    strcpy(sa.sun_path, path);
    unlink(path);

    if (bind(local_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        perror("bind failed");
        exit(1);
    }
    if (listen(local_socket, MAX_CLIENTS) < 0) {
        perror("listen failed");
        exit(1);
    }

}

void init_network(int port)
{
    if ((network_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
        perror("init of network socket failed");
        exit(1);
    }

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = INADDR_ANY;

    if (bind(network_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        perror("bind failed");
        exit(1);
    }
    if (listen(network_socket, MAX_CLIENTS) < 0) {
        perror("listen failed");
        exit(1);
    }

}

int get_active()
{

     struct pollfd *sockets = malloc((number_of_clients + 2) * sizeof(struct pollfd));
    sockets[0].fd = network_socket;
    sockets[1].fd = local_socket;
    sockets[0].events = POLLIN;
    sockets[1].events = POLLIN;

    pthread_mutex_lock(&mutex);
    for (int i = 2; i < number_of_clients + 2; i++) {
        sockets[i].fd = clients[i - 2].socket;
        sockets[i].events = POLLIN;
    }
    pthread_mutex_unlock(&mutex);
    poll(sockets, 2 + number_of_clients, -1); 

    for (int i = 0; i < number_of_clients + 2; i++) {
        if (sockets[i].revents == POLLIN) {
            int socket = sockets[i].fd;
            if (socket == local_socket || socket == network_socket) {
                return accept(socket, NULL, NULL);
            }
            return socket;
        }
    }

    free(sockets);
    return -1;
}

int find_opponent(int socket, char * msg)
{
    int was = -1;
    for (int i = 0; i < number_of_clients; i++) {
        if (clients[i].socket != socket && clients[i].opponentSocket == -1) {
            was = 1;
            clients[i].opponentSocket = socket;
            clients[number_of_clients - 1].opponentSocket = clients[i].socket;
            send(socket, "X", MSG_LEN, 0);
            send(clients[i].socket, "O", MSG_LEN, 0);
        }
    }
    return was;
}

void add_new_client(int socket, char * msg)
{
    
    clients[number_of_clients].socket = socket;
    strcpy(clients[number_of_clients].name, msg);
    clients[number_of_clients].is_alive = 1;
    clients[number_of_clients].opponentSocket = -1;
    number_of_clients++;
    printf("[REGISTERED] %s\n", msg);
   
    if (find_opponent(socket, msg)<0) {
        send(socket, "NO", MSG_LEN, 0);
    }
}

void remove_dead_clients() {
    for (int i = 0; i < number_of_clients; i++) {
        if (!clients[i].is_alive) {
            if (i < number_of_clients - 1) {
                clients[i] = clients[number_of_clients - 1];
            }
            else{
                clients[i].is_alive=-1;
                strcpy(clients[i].name, " ");
                clients[i].socket = -1;
                clients[i].opponentSocket =-1;

            }
            number_of_clients--;
        }
    }
}

void* ping_service(){
    while(1){
        sleep(5);
        pthread_mutex_lock(&mutex);
        for(int i = 0; i < MAX_CLIENTS; i++){
            if(clients[i].socket != -1){
                char buff[MSG_LEN];
                clients[i].is_alive =0;
                sprintf(buff, "%s", "P");
                send(clients[i].socket, buff, MSG_LEN, 0);
            }
        }
        pthread_mutex_unlock(&mutex);
        sleep(15);
        pthread_mutex_lock(&mutex);

        for(int i = 0; i < MAX_CLIENTS; i++){
            if(clients[i].is_alive == 0 ){
                printf("No ping response from: %d\n", i);
                for(int j=0;j<MAX_CLIENTS;j++)
                {
                    if(clients[i].opponentSocket == clients[j].socket)
                        clients[j].is_alive=0;
                }
                remove_dead_clients();
                }
        }
        pthread_mutex_unlock(&mutex);
    }
}


void listen_to_clients()
{
    while (1) {
        int socket = get_active();
        char msg[MSG_LEN];
        if (recv(socket, msg, MSG_LEN, 0) <= 0) {
            sleep(1);
        }

        pthread_mutex_lock(&mutex);
        if (strcmp(msg, "P") == 0) 
        {
            for (int i = 0; i < number_of_clients; i++) {
                if (clients[i].socket == socket) {
                    clients[i].is_alive = 1;
                }
            }
        } 
        else if (strlen(msg) == 1 && msg[0] >= '1' && msg[0] <= '9') 
        {
            printf("Move\n");
            for (int i = 0; i < number_of_clients; i++) {
                if (clients[i].socket == socket) {
                    if (clients[i].opponentSocket == -1) {
                        send(socket, "NO", MSG_LEN, 0);
                    } else {
                        send(clients[i].opponentSocket, msg, MSG_LEN, 0);
                    }
                }
            }
        } 
        else if (strlen(msg) == 1 && (msg[0] == 'W' || msg[0] == 'L')) 
        {
            printf("End\n");
            for (int i = 0; i < number_of_clients; i++) {
                if (clients[i].socket == socket) {
                    if (clients[i].opponentSocket == -1) {
                        clients[i].is_alive = 0;
                    } else {
                        send(clients[i].opponentSocket, msg, MSG_LEN, 0);
                        clients[i].is_alive = 0;
                        for (int j = 0; j < number_of_clients; j++) {
                            if (clients[j].socket == clients[i].opponentSocket) {
                                clients[j].is_alive = 0;
                            }
                        }
                    }
                    remove_dead_clients();
                }
            }
        } 
        else 
        {
            printf("%s\n", msg);
            int is_new = 1;
            for (int i = 0; i < number_of_clients; i++) {
                if (clients[i].socket == socket) {
                    is_new = 0;
                } else if (strcmp(msg, clients[i].name) == 0) {
                    is_new = 0;
                    printf("REGISTERED failed, name %s taken\n", msg);
                    send(socket, "NT", MSG_LEN, 0);
                }
            }
            if (is_new == 1) 
                add_new_client(socket, msg);   
        }
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char **argv)
{
    if(argc<3)
        perror("Too few args");

    signal(SIGPIPE, SIG_IGN);

    number_of_clients=0;

    init_local(argv[2]);
    init_network(atoi(argv[1]));

    pthread_t ping;
    pthread_create(&ping, NULL, &ping_service, NULL);

    printf("local %d, network %d\n", local_socket, network_socket);

    listen_to_clients();

    pthread_join(ping, NULL);

    return 0;
}