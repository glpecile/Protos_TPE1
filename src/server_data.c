#include <time.h>
#include <string.h>
#include <stdio.h>

#include "../include/server_data.h"
static int correct_lines;
static int incorrect_lines;
static int incorrect_datagrams;
static int connections;
static enum date_format locale = EN;
void echo(char * dest, char * s, int position){
    int len = strlen(s + position + 1);
    memcpy(dest, s + position + 1, len + 1);
}
void get_date(char * dest){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    if(locale == ES)
        sprintf(dest, "%02d-%02d-%d\r\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    else
        sprintf(dest, "%02d-%02d-%d\r\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}
void get_time(char * dest){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(dest, "%02d:%02d:%02d\r\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
}
void set_locale(char * dest, const enum date_format l){
    locale = l;
    sprintf(dest, "Date format changed\r\n");
}
void stats(char * dest){
    sprintf(dest, "STATS:\n-Connections: %d\n-Incorrect Lines: %d\n-Correct Lines: %d\n-Incorrect Datagrams: %d\r\n", connections, incorrect_lines, correct_lines, incorrect_datagrams);
}

int post_correct_lines(){
    return correct_lines++;
}

int post_incorrect_lines(){
    return incorrect_lines++;
}

int post_incorrect_datagrams(){
    return incorrect_datagrams++;
}

int post_connections(){
    return connections++;
}