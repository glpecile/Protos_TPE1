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
#include "../include/logger.h"


#define DEFAULT_PORT 9999 //Prefered port
#define MAX_ARGS  2
#define MAX_CLIENTS 30

int main(int argc, char* argv[]){

    if(argc > MAX_ARGS ) {
        log(FATAL, "usage: %s <Server Port/Name>", argv[0]);//exits from the execution if it receives FATAL as an argument.
    }
    //Defining port: DEFAULT_PORT or the port given by the user.
    int port = (argc == 1) ? DEFAULT_PORT : atoi(argv[1]);

    //Buffer of 1K
    char buffer[1025];

    int opt = TRUE;
    int master_socket , addrlen , new_socket , max_sd , activity, i , valread , sd;
    int client_socket[MAX_CLIENTS] = {0};
    struct sockaddr_in address;
    struct addrinfo hint;
    //set of socket descriptors
    fd_set readfds;

    if(close(STDIN_FILENO))
        log(FATAL,"closing STDIN failed.");

    //master socket TCP created.
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) < 0)
    {
        log(FATAL,"socket failed");
    }

    //set master socket to allow multiple connections.
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        log(FATAL,"setsockopt");
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );





}