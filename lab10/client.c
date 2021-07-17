#include "utils.h"

char *my_name;
int server_socket;
char my_figure;
char opponent_figure;
int number_of_moves=0;
int my_moves[4]={-1,-1,-1,-1};
int opponent_moves[4]={-1,-1,-1,-1};
char moves[9]={'-','-','-','-','-','-','-','-','-'} ;
int count_my_moves =0;
int count_op_moves =0;


void connect_to_server(char *type, char *address) {
    if (strcmp(type, "local") == 0) {

        if ((server_socket = socket(AF_UNIX, SOCK_STREAM, 0)) <= 0) {
            perror("local socket failed");
            exit(1);
        }
        struct sockaddr_un sa;
        sa.sun_family = AF_UNIX;
        strcpy(sa.sun_path, address);
        if (connect(server_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
            perror("connect local failed");
            exit(1);
        }

    } else if (strcmp(type, "network") == 0) {

        int port = atoi(address);
        if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
            perror("network socket failed");
            exit(1);
        }
        struct sockaddr_in sa;
        sa.sin_family = AF_INET;
        sa.sin_port = htons(port);
        sa.sin_addr.s_addr = INADDR_ANY;
        if (connect(server_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
            perror("connect network failed");
            exit(1);
        }

    } else {
        exit(1);
    }
}

void register_in_server() {
    char buff[MSG_LEN];
    sprintf(buff, "%s", my_name);
    send(server_socket, buff, MSG_LEN, 0);
}

void display() {

    for(int i =0;i<4;i++)
        if(my_moves[i]!=-1)
            moves[my_moves[i]-1]=my_figure;


    for(int i =0;i<4;i++)
        if(opponent_moves[i]!=-1)
            moves[opponent_moves[i]-1]=opponent_figure;

    for(int i =1;i<10;i++)
    {
        printf(" %c ", moves[i-1]);
        if(i%3==0)
            printf("\n");
    }
}

void check_game() 
{
    char winning ='-';

    //normal
    for(int i=0;i<3;i++)
        if(moves[i] == moves[i+3] && moves[i]== moves[i+6] && moves[i]!='-')
            winning = moves[i];

    for(int i=0;i<9;i+=3)
        if(moves[i]==moves[i+1] && moves[i+1] == moves[i+2] && moves[i]!= '-')
            winning = moves[i];


    //diagonals
    if(moves[0] == moves[4] && moves[4]== moves[8] && moves[0]!='-')
            winning = moves[0];

    if(moves[2] == moves[4] && moves[4]== moves[6] && moves[2]!='-')
            winning = moves[2];
        

    if(winning!='-')
    {
        printf("I WON: %c\n", winning);
        send(server_socket, "L", MSG_LEN, 0);
        exit(0);
    }
}

void make_move() {
    
    printf("Your move: ");
    int m;
    scanf("%d", &m);

    for(int i =0;i<4;i++)
    {
        if(my_moves[i]==m)
        {
            printf("PLACE TAKEN\n");
            send(server_socket,"W", MSG_LEN, 0);
            exit(1);
        }    
    }

    for(int i =0;i<4;i++)
    {
        if(opponent_moves[i]==m)
        {
            printf("PLACE TAKEN\n");
            send(server_socket,"W", MSG_LEN, 0);
            exit(1);
        }    
    }
    
    my_moves[count_my_moves] = m;
    count_my_moves++;
    display();
    check_game();
    char buff[MSG_LEN];
    sprintf(buff, "%d", m);
    send(server_socket, buff, MSG_LEN, 0);

}

void server_listen() {
    char msg[MSG_LEN];

    struct pollfd *sockets = malloc(sizeof(struct pollfd));
    sockets->fd = server_socket;
    sockets->events = POLLIN;

    while (1) {
        poll(sockets, 1, -1);

        recv(server_socket, msg, MSG_LEN, 0);
        if (strcmp(msg, "P") == 0) {
            printf("PING\n");
            send(server_socket, "P", MSG_LEN, 0);
            continue;
        } else if (strcmp(msg, "NT") == 0) {
            printf("Name taken");
            exit(0);
        } else if (strcmp(msg, "NO") == 0) {
            printf("Waiting for the opponent ...\n");
        } else if (strcmp(msg, "X") == 0) {
            my_figure = 'X';
            opponent_figure = 'O';
            printf("My figure %c\n", my_figure);
        } else if (strcmp(msg, "O") == 0) {
            my_figure = 'O';
            opponent_figure = 'X';
            printf("My figure %c\n", my_figure);
            display();
            make_move();
        } else if (strlen(msg) == 1 && msg[0] >= '1' && msg[0] <= '9') {
            int move = msg[0] - '0';
            printf("move: %d\n", move);
            
            opponent_moves[count_op_moves]=move;
            count_op_moves++;

            display();
            make_move();
        } else if (strlen(msg) == 1 && msg[0] == 'W') {
            printf("I WON\n");
            exit(0);
        } else if (strlen(msg) == 1 && msg[0] == 'L') {
            printf("I LOSE\n");
            exit(0);
        }  else {
            printf("%s\n", msg);
        }
    }
}


int main(int argc, char ** argv)
{
    if(argc<4)
        perror("Too few args");

    my_name = argv[1];
    printf("%s\n", my_name);

    connect_to_server(argv[2], argv[3]);

    printf("connected\n");
    register_in_server();
    printf("registered\n");

    printf("server socket: %d\n", server_socket);
    display();
    
    server_listen();

}