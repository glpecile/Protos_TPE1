#ifndef PROTOS_TPE1_CLIENT_H
#define PROTOS_TPE1_CLIENT_H

#include "../include/client.h"

#define BUFF_SIZE 103
#define BUFF_LIMIT BUFF_SIZE - 3
#define MAX_CLIENTS 30
#define US_ASCII_LIMIT 127

typedef struct {
    char buffer[BUFF_SIZE];
    int index;
    int fd;
} t_client;

t_client *init_client(int fd);

void destroy_client(t_client *client);

int write_client(t_client *client, char c);

char *read_client(t_client *client);

int is_invalid(t_client *client);

#endif //PROTOS_TPE1_CLIENT_H
