#ifndef PROTOS_TPE1_UDP_UTILS_H
#define PROTOS_TPE1_UDP_UTILS_H

#include "server_data.h"

int setup_udp_server_socket(const int port, struct sockaddr_in *address);

void handle_udp_datagrams(fd_set *readfds, int udp_socket);


#endif //PROTOS_TPE1_UDP_UTILS_H
