

#ifndef PROTOS_TPE1_EXECUTIONER_H
#define PROTOS_TPE1_EXECUTIONER_H
#include "parser_utils.h"
#include <string.h>
#include <stdlib.h>
#define MAX_CMD 3
#define TRUE 1
#define FALSE 0
#define CMD_ECHO "echo "
#define CMD_GET_DATE "get date"
#define CMD_GET_TIME "get time"
//enum command_types{
//    ECHO,
//    GET_DATE,
//    GET_TIME,
//};

void init_executioner();
const char * execute(char * string);
void reset_parser_executioner();
void destroy_executioner();
#endif //PROTOS_TPE1_EXECUTIONER_H
