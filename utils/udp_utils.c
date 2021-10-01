#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close

#include "../include/udp_utils.h"
#include "../include/server_utils.h"

int setup_udp_server_socket(const int port, struct sockaddr_in *address) {
    return setup_server_socket(port, address, SOCK_DGRAM);
}