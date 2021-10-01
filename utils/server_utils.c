#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close

#include "../include/server_utils.h"
#include "../include/tools.h"
#include "../include/logger.h"


int setup_server_socket(const int port, struct sockaddr_in *address, int sock_type) {
    int opt = TRUE;
    int master_socket;

    //master socket TCP created.
    if ((master_socket = socket(AF_INET, sock_type, 0)) < 0) {
        log(FATAL, "socket failed");
    }

    //set master socket to allow multiple connections.
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0) {
        log(FATAL, "setsockopt");
    }

    //type of socket address created
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(port);

    //bind the master socket.
    if (bind(master_socket, (struct sockaddr *) address, sizeof(*address)) < 0) {
        if(close(master_socket) < 0) {
            log(FATAL, "close failed")
        }
        log(FATAL, "bind failed")
    }
    printf("Listener on port %d\n", port);

    return master_socket;
}