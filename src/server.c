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
#include "../include/udp_utils.h"

#define DEFAULT_PORT 9999 //Preferred port
#define MAX_ARGS  2
#define MAX_CLIENTS 30

#define MAXSTRINGLENGTH 100 //TODO Borrar

static t_client *clients[MAX_CLIENTS] = {NULL};

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    if (argc > MAX_ARGS) {
        log(FATAL, "usage: %s <Server Port/Name>",
            argv[0]); //exits from the execution if it receives FATAL as an argument.
    }
    //Defining port: DEFAULT_PORT or the port given by the user.
    int port = (argc == 1) ? DEFAULT_PORT : atoi(argv[1]);

    int master_socket, max_sd, max_tcp_sd, activity;
    int udp_socket;
    struct sockaddr_in tcp_address;
    struct sockaddr_in udp_address;
    int tcp_addrlen;
    int udp_addrlen;
    fd_set readfds; //set of socket descriptors


    if (close(STDIN_FILENO)) {
        log(FATAL, "closing STDIN failed.")
    }

    //Initialize tcp master_socket
    master_socket = setup_tcp_server_socket(port, &tcp_address);

    //Initialize udp socket
    udp_socket = setup_udp_server_socket(port, &udp_address);

    //initialize executioner
    init_executioner();

    //accept the incoming connection
    tcp_addrlen = sizeof(tcp_address);
    udp_addrlen = sizeof(udp_address);
    puts("Waiting for connections ...");

    while (TRUE) {
        max_tcp_sd = fill_set(master_socket, &readfds, clients);
        FD_SET(udp_socket, &readfds);
        max_sd = max_tcp_sd > udp_socket ? max_tcp_sd : udp_socket;
        //waiting for one of the sockets, timeout is NULL.
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            log(FATAL, "select error.");
        }

        //If something happened on the master socket, then its an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {
            handle_incoming_connection(master_socket, &tcp_address, tcp_addrlen, clients);
        }

        //ISSET UDP -> TODA SU LOGICA

        handle_tcp_clients(&readfds, &tcp_address, tcp_addrlen, clients);

        int n;
        if(FD_ISSET(udp_socket, &readfds)) {
            struct sockaddr_storage clntAddr; 			// Client address
            // Set Length of client address structure (in-out parameter)
            socklen_t clntAddrLen = sizeof(clntAddr);

            // Block until receive message from a client
            char buffer[MAXSTRINGLENGTH];

            errno = 0;
            // Como alternativa a recvfrom se puede usar recvmsg, que es mas completa, por ejemplo permite saber
            // si el mensaje recibido es de mayor longitud a MAXSTRINGLENGTH
            ssize_t numBytesRcvd = recvfrom(udp_socket, buffer, MAXSTRINGLENGTH, 0, (struct sockaddr *) &clntAddr, &clntAddrLen);
            if (numBytesRcvd < 0) {
                log(ERROR, "recvfrom() failed: %s ", strerror(errno))
                continue;
            }

            // Send received datagram back to the client
            ssize_t numBytesSent = sendto(udp_socket, buffer, numBytesRcvd, 0, (struct sockaddr *) &clntAddr, sizeof(clntAddr));
            if (numBytesSent < 0)
            log(ERROR, "sendto() failed")
            else if (numBytesSent != numBytesRcvd)
            log(ERROR, "sendto() sent unexpected number of bytes");
        }
    }
    return 0;
}