#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include "../include/tcp_utils.h"
#include "../include/logger.h"
#include "../include/tools.h"

#define PENDING_CONNECTIONS 3

int setup_tcp_server_socket(const int port) {
    int opt = TRUE;
    int master_socket;
    struct sockaddr_in address;

    //master socket TCP created.
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log(FATAL, "socket failed");
    }

    //set master socket to allow multiple connections.
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0) {
        log(FATAL, "setsockopt");
    }

    //type of socket address created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    //bind the master socket.
    if (bind(master_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        log(FATAL, "bind failed")
    }
    printf("Listener on port %d\n", port);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, PENDING_CONNECTIONS) < 0) {
        log(FATAL, "listen");
    }

    return master_socket;
}
