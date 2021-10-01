#ifndef PROTOS_TPE1_TCP_UTILS_H
#define PROTOS_TPE1_TCP_UTILS_H

#include "../include/client.h"

int setup_tcp_server_socket(const int port, struct sockaddr_in * address);

int fill_set(int master_socket, fd_set * readfds, t_client *clients[]);

void handle_incoming_connection(int master_socket, struct sockaddr_in * address, int addrlen, t_client *clients[], int * current_tcp_clients);

void handle_tcp_clients(fd_set * readfds, struct sockaddr_in * address, int addrlen, t_client *clients[], int * current_tcp_clients);

#endif //PROTOS_TPE1_TCP_UTILS_H
