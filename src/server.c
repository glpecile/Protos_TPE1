#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <netinet/in.h>
#include <netdb.h>
#include "../include/tools.h"
#include "../include/logger.h"
#include "../include/executioner.h"
#include "../include/client.h"


#define DEFAULT_PORT 9999 //Preferred port
#define MAX_ARGS  2
#define MAX_CLIENTS 30
#define PENDING_CONNECTIONS 3
#define SET_EMPTY -1 //used init for client fd
#define IS_EMPTY(sd) (sd == SET_EMPTY)
static t_client * clients[MAX_CLIENTS] = {NULL};
static void clean_read_buffer(int fd){
    char buffer = ' ';
    char old_buffer = ' ';
    while(!(buffer == '\n' && old_buffer == '\r')) {
        old_buffer = buffer;
        read(fd, &buffer, 1);
    }
}
int main(int argc, char* argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    if(argc > MAX_ARGS ) {
        log(FATAL, "usage: %s <Server Port/Name>", argv[0]); //exits from the execution if it receives FATAL as an argument.
    }
    //Defining port: DEFAULT_PORT or the port given by the user.
    int port = (argc == 1) ? DEFAULT_PORT : atoi(argv[1]);

    //Buffer of 100
    char buffer[BUFF_SIZE];

    //initialize executioner
    init_executioner();

    int opt = TRUE;
    int master_socket , addrlen , new_socket , max_sd , activity, i , valread , sd;
    int client_socket[MAX_CLIENTS] = {0};
    struct sockaddr_in address;
    //set of socket descriptors
    fd_set readfds;



    char * message = "ECHO Daemon v1.0\r\n";

    if(close(STDIN_FILENO)) {
        log(FATAL,"closing STDIN failed.")
    }

    //initialise all client_socket[] to -1 so at first is not added to the FDSET
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        client_socket[i] = SET_EMPTY;
    }

    //master socket TCP created.
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) < 0) {
        log(FATAL,"socket failed");
    }

    //set master socket to allow multiple connections.
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ) {
        log(FATAL,"setsockopt");
    }

    //type of socket address created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    //bind the master socket.
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) {
        log(FATAL,"bind failed")
    }
    printf("Listener on port %d\n", port);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, PENDING_CONNECTIONS) < 0) {
        log(FATAL, "listen");
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while(TRUE) {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for (i = 0; i < MAX_CLIENTS; i++) {
            //socket descriptor
//            sd = client_socket[i];
            if(clients[i] != NULL) {
                sd = clients[i]->fd;
                //if valid socket descriptor then read list
                if (!IS_EMPTY(sd)) {
                    FD_SET(sd, &readfds);
                }

                //highest file descriptor number, need it for the select function
                if (sd > max_sd) {
                    max_sd = sd;
                }
            }

        }
        //waiting for one of the sockets, timeout is NULL.
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            log(FATAL, "select error.");
        }

        //If something happened on the master socket, then its an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
                log(FATAL, "accept failed.");
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection, socket fd is %d, ip is: %s, port: %d \n", new_socket, inet_ntoa(address.sin_addr),
                   ntohs(address.sin_port));

            //send new connection greeting message
            if (send(new_socket, message, strlen(message), 0) != strlen(message)) {
                log(FATAL, "Send error");
            }

            printf("Welcome message sent successfully\n");

            //add new socket socket to array of sockets
            for (i = 0; i < MAX_CLIENTS; i++) {
                //if position is empty
//                if (IS_EMPTY(client_socket[i])) {
//                    client_socket[i] = new_socket;
//                    printf("Adding to list of sockets as %d\n", i);
//                    break;
//                }
                if (clients[i] == NULL) {
                    clients[i] = init_client(new_socket);
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }

        //ISSET UDP -> TODA SU LOGICA

        //if it's some IO operation regarding one of the "old" clients.
        for (i = 0; i < MAX_CLIENTS; i++) {
//            sd = client_socket[i];
            if(clients[i] != NULL){
                sd = clients[i]->fd;
                if (FD_ISSET(sd, &readfds)) {
                    char c;
                    //check if it was for closing, and also the incoming message
                    valread = read(sd, &c, 1);
                    //clean_read_buffer(sd);
                    if (valread  <= 0) { //CHECKEAR PORQUE ACA LO CAMBIAMOS DE == 0 A < 0
                        //A client got disconnected, print details.
                        getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                        printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

                        //Close the socket and mark as SET_EMPTY in list for reuse
                        close(sd);
                        client_socket[i] = SET_EMPTY;
                        destroy_client(clients[i]);
                        clients[i] = NULL;
                    }
                        //echo back the message that came in
                    else {
                        printf("buff_size: %d char: %c\n", clients[i]->index, c);
                        if(write_client(clients[i],c)){
                            const char * to_send;
                            char * read_ret = read_client(clients[i]);
                            if(read_ret != NULL) {
                                to_send = execute(read_ret);
                                send(sd, to_send, strlen(to_send), 0);
                                reset_parser_executioner();
                            }
//                            if(is_full(clients[i])){
//                                send(sd, "buffer is full\n", 16, 0);
//                                while(read(sd,&c,1)>0 && c!='\n'){
//                                    printf("limpiando el ascii: %d\n", c);
//                                }
//                                send(sd, "SALIMO DEL GUAIL\n", 16, 0);
//                            }

                            //set the string terminating NULL byte on the end of the data read
                        }
                    }
                }
            }
        }
    }
    return 0;
}