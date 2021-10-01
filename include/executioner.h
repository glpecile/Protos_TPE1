

#ifndef PROTOS_TPE1_EXECUTIONER_H
#define PROTOS_TPE1_EXECUTIONER_H

#include "parser_utils.h"
#include "server_data.h"
#include <string.h>
#include <stdlib.h>


#define MAX_CMD_TCP 3
#define MAX_CMD_UDP 3
#define TRUE 1
#define FALSE 0
#define CMD_ECHO "echo "
#define CMD_GET_DATE "get date"
#define CMD_GET_TIME "get time"
#define CMD_SET_LOCALE_ES "set locale es"
#define CMD_SET_LOCALE_EN "set locale en"
#define CMD_STATS "stats"

#define US_ASCII_LIMIT 127
enum command_types{
    ECHO,
    GET_DATE,
    GET_TIME,
    SET_LOCALE_EN,
    SET_LOCALE_ES,
    STATS
};
enum connection_type{
    TCP,
    UDP
};


void init_executioner();

const char *execute(char *string, const enum connection_type con_type);

void reset_parser_executioner();

void destroy_executioner();

#endif //PROTOS_TPE1_EXECUTIONER_H
