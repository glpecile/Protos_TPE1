
#ifndef PROTOS_TPE1_SERVER_DATA_H
#define PROTOS_TPE1_SERVER_DATA_H
#include <time.h>
#include <string.h>
#include <stdio.h>
enum date_format{
    EN,
    ES
};
void echo(char * dest, char * s, int position);
void get_date(char * dest);
void get_time(char * dest);
void set_locale(char * dest, const enum date_format locale);
void stats(char * dest);
int get_correct_lines();
int post_correct_lines();
int get_incorrect_lines();
int post_incorrect_lines();
int get_incorrect_datagrams();
int post_incorrect_datagrams();
int get_connections();
int post_connections();

#endif //PROTOS_TPE1_SERVER_DATA_H
