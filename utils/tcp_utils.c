#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include "../include/tcp_utils.h"
#include "../include/logger.h"
#include "../include/tools.h"
#include "../include/client.h"
#include "../include/executioner.h"
#include "../include/server_utils.h"
#include "../include/server_data.h"

#define PENDING_CONNECTIONS 3

#define SET_EMPTY -1 //used init for client fd
#define IS_EMPTY(sd) (sd == SET_EMPTY)

int setup_tcp_server_socket(const int port, struct sockaddr_in *address) {
    int master_socket = setup_server_socket(port, address, SOCK_STREAM);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, PENDING_CONNECTIONS) < 0) {
        log(FATAL, "listen");
    }

    return master_socket;
}

int fill_set(int master_socket, fd_set *readfds, t_client *clients[], int *current_tcp_clients) {
    int sd, max_sd = 0;

    //clear the socket set
    FD_ZERO(readfds);

    //add master socket to set
    if (*current_tcp_clients < MAX_CLIENTS) {
        FD_SET(master_socket, readfds);
        max_sd = master_socket;
    }

    //add child sockets to set
    for (int i = 0; i < MAX_CLIENTS; i++) {
        //socket descriptor
        if (clients[i] != NULL) {
            sd = clients[i]->fd;
            //if valid socket descriptor then read list
            if (!IS_EMPTY(sd)) {
                FD_SET(sd, readfds);
            }
            //highest file descriptor number, need it for the select function
            if (sd > max_sd) {
                max_sd = sd;
            }
        }
    }
    return max_sd;
}

void handle_incoming_connection(int master_socket, struct sockaddr_in *address, int addrlen, t_client *clients[],
                                int *current_tcp_clients) {
    int new_socket;
    char *message = "ECHO Daemon v1.0\r\n";

    if ((new_socket = accept(master_socket, (struct sockaddr *) address, (socklen_t *) &addrlen)) < 0) {
        log(FATAL, "accept failed.");
    }

    //inform user of socket number - used in send and receive commands
    log(INFO, "New connection, socket fd is %d, ip is: %s, port: %d", new_socket, inet_ntoa(address->sin_addr),
        ntohs(address->sin_port));

    //send new connection greeting message
    if (send(new_socket, message, strlen(message), 0) != strlen(message)) {
        log(FATAL, "Send error");
    }

    //add new socket socket to array of sockets
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == NULL) {
            clients[i] = init_client(new_socket);
            post_connections();
            (*current_tcp_clients)++;
            break;
        }
    }
}

void handle_tcp_clients(fd_set *readfds, struct sockaddr_in *address, int addrlen, t_client *clients[],
                        int *current_tcp_clients) {
    int sd, valread;
    //if it's some IO operation regarding one of the "old" clients.
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != NULL) {
            sd = clients[i]->fd;
            if (FD_ISSET(sd, readfds)) {
                char c;
                //check if it was for closing, and also the incoming message
                valread = read(sd, &c, 1);
                if (valread <= 0) {
                    //A client got disconnected, print details.
                    getpeername(sd, (struct sockaddr *) address, (socklen_t *) &addrlen);
                    log(INFO, "Host disconnected , ip %s , port %d \n", inet_ntoa(address->sin_addr),
                        ntohs(address->sin_port));

                    //Close the socket and mark as NULL in list for reuse
                    close(sd);
                    destroy_client(clients[i]);
                    clients[i] = NULL;
                    (*current_tcp_clients)--;
                } else {
                    int send_flag = write_client(clients[i], c);
                    if (is_invalid(clients[i])) {
                        while (c != '\n' && read(sd, &c, 1) > 0);
                    }
                    if (send_flag) {
                        const char *to_send;
                        char *read_ret = read_client(clients[i]);
                        if (read_ret != NULL) {
                            to_send = execute(read_ret, TCP);
                            send(sd, to_send, strlen(to_send), 0);
                            reset_parser_executioner(TCP);
                        }
                    }
                }
            }
        }
    }
}
