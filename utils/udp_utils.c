#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close

#include "../include/udp_utils.h"
#include "../include/server_utils.h"
#include "../include/logger.h"
#include "../include/executioner.h"
#include "../include/server_data.h"

#define BUFF_SIZE 100

int setup_udp_server_socket(const int port, struct sockaddr_in *address) {
    return setup_server_socket(port, address, SOCK_DGRAM);
}

void handle_udp_datagrams(fd_set *readfds, int udp_socket) {
    if (FD_ISSET(udp_socket, readfds)) {
        struct sockaddr_storage clntAddr;            // Client address
        // Set Length of client address structure (in-out parameter)
        socklen_t clntAddrLen = sizeof(clntAddr);

        // Block until receive message from a client
        char buffer[BUFF_SIZE] = {0};
        const char *answer;

        errno = 0;
        ssize_t valread = recvfrom(udp_socket, buffer, BUFF_SIZE, 0, (struct sockaddr *) &clntAddr, &clntAddrLen);
        if (valread < 0) {
            log(ERROR, "recvfrom() failed: %s ", strerror(errno));
        } else if (valread >= BUFF_SIZE) {
            log(ERROR, "recvfrom() failed: Datagram exceed max supported size");
            post_incorrect_datagrams();
        } else {
            answer = execute(buffer, UDP);
            int bytes_to_sent = strlen(answer);

            ssize_t numBytesSent = sendto(udp_socket, answer, bytes_to_sent, 0, (struct sockaddr *) &clntAddr,
                                          sizeof(clntAddr));
            reset_parser_executioner(UDP);
            if (numBytesSent < 0) {
                log(ERROR, "sendto() failed");
            } else if (numBytesSent != bytes_to_sent) {
                log(ERROR, "sendto() sent unexpected number of bytes");
            }
        }
    }

}
