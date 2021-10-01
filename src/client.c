#include "../include/client.h"
t_client * init_client(int fd){
    t_client * ret = malloc(sizeof(*ret));
    if(ret != NULL){
        memset(ret, 0, sizeof(*ret));
        ret->fd = fd;
        ret->index = 0;
    }
    return ret;
}
void destroy_client(t_client * client){
    if(client != NULL)
        free(client);
}

int write_client(t_client * client, char c){
    //falta chequear cant de char de buffer.
    if(client != NULL){
        int send = FALSE;
        if(c >= US_ASCII_LIMIT || (client->index >= BUFF_LIMIT && c!='\r' && c!='\n')){
            send = TRUE;
            client->buffer[client->index ++] = '\r';
            client->buffer[client->index ++] = '\n';
            client->buffer[client->index + 1] = '\0';
        }else{
            client->buffer[client->index] = c;
            if((client->index != 0 && (c == '\n' && client->buffer[client->index-1] == '\r'))){
                client->buffer[client->index+1]='\0';
                send = TRUE;
            }
        }
        client->index++;
        return send;
    }
    return 0;
}

char * read_client(t_client * client){
    if(client != NULL){
        client->index = 0;
        return client->buffer;
    }
    return NULL;
}

int is_full(t_client * client){
    return client->index == BUFF_SIZE;
}

