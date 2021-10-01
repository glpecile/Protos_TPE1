#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include "../include/tools.h"
#include "../include/logger.h"
#include "../include/executioner.h"
#include "../include/client.h"
#include "../include/tcp_utils.h"
#include "../include/udp_utils.h"

#define DEFAULT_PORT 9999 //Preferred port
#define MAX_ARGS  2

int main(int argc, char *argv[]) {
    if (argc > MAX_ARGS) {
        log(FATAL, "usage: %s <Server Port/Name>",
            argv[0]); //exits from the execution if it receives FATAL as an argument.
    }
    //Defining port: DEFAULT_PORT or the port given by the user.
    int port = (argc == 1) ? DEFAULT_PORT : atoi(argv[1]);

    int master_socket;
    int current_tcp_clients = 0;
    int udp_socket;
    struct sockaddr_in tcp_address;
    struct sockaddr_in udp_address;
    int tcp_addrlen;
    fd_set readfds; //set of socket descriptors
    t_client *clients[MAX_CLIENTS] = {NULL};

    if (close(STDIN_FILENO)) {
        log(FATAL, "closing STDIN failed.")
    }

    //Initialize tcp master_socket
    master_socket = setup_tcp_server_socket(port, &tcp_address);

    //Initialize udp socket
    udp_socket = setup_udp_server_socket(port, &udp_address);

    //initialize executioner
    init_executioner();

    tcp_addrlen = sizeof(tcp_address);

    log(INFO, "Waiting for connections ...");

    while (TRUE) {
        int max_tcp_sd = fill_set(master_socket, &readfds, clients, &current_tcp_clients);
        FD_SET(udp_socket, &readfds);
        int max_sd = max_tcp_sd > udp_socket ? max_tcp_sd : udp_socket;
        //waiting for one of the sockets, timeout is NULL.
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            log(FATAL, "select error.");
        }

        //If something happened on the master socket, then its an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {
            handle_incoming_connection(master_socket, &tcp_address, tcp_addrlen, clients, &current_tcp_clients);
        }

        handle_tcp_clients(&readfds, &tcp_address, tcp_addrlen, clients, &current_tcp_clients);

        handle_udp_datagrams(&readfds, udp_socket);
    }

    destroy_executioner();
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != NULL) {
            destroy_client(clients[i]);
        }
    }
    return 0;
}