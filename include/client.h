#ifndef PROTOS_TPE1_CLIENT_H
#define PROTOS_TPE1_CLIENT_H
#include <string.h>
#include <stdlib.h>
#define BUFF_SIZE 110
#define MAX_CLIENTS 30
#define TRUE 1
#define FALSE 0
typedef struct {
    char buffer[BUFF_SIZE];
    int index;
    int fd;
}t_client;

t_client * init_client(int fd);
void destroy_client(t_client * client);
int write_client(t_client * client, char c);
char * read_client(t_client * client);
int is_full(t_client * client);

#endif //PROTOS_TPE1_CLIENT_H
