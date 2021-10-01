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
#include "../include/tcp_utils.h"


#define DEFAULT_PORT 9999 //Preferred port
#define MAX_ARGS  2
#define MAX_CLIENTS 30


static t_client *clients[MAX_CLIENTS] = {NULL};

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    if (argc > MAX_ARGS) {
        log(FATAL, "usage: %s <Server Port/Name>",
            argv[0]); //exits from the execution if it receives FATAL as an argument.
    }
    //Defining port: DEFAULT_PORT or the port given by the user.
    int port = (argc == 1) ? DEFAULT_PORT : atoi(argv[1]);

    int master_socket, addrlen, new_socket, max_sd, activity, i, valread, sd;
    int udp_socket;
    struct sockaddr_in address;
    fd_set readfds; //set of socket descriptors


    if (close(STDIN_FILENO)) {
        log(FATAL, "closing STDIN failed.")
    }

    //Initialize tcp master_socket
    master_socket = setup_tcp_server_socket(port, &address);

    //initialize executioner
    init_executioner();

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while (TRUE) {
        max_sd = fill_set(master_socket, &readfds, clients);

        //waiting for one of the sockets, timeout is NULL.
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            log(FATAL, "select error.");
        }

        //If something happened on the master socket, then its an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {
            handle_incoming_connection(master_socket, &address, addrlen, clients);
        }

        //ISSET UDP -> TODA SU LOGICA

        handle_tcp_clients(&readfds, &address, addrlen, clients);
    }
    return 0;
}