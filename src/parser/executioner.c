#include "../../include/executioner.h"
static int to_parse[MAX_CMD];
static const struct parser_event *  parser_events[MAX_CMD];
static struct parser * parsers[MAX_CMD];
static struct parser_definition echo_d;
static struct parser_definition get_date_d;
static struct parser_definition get_time_d;
static char toReturn[100];
static const char * call_to_action(const enum command_types type, char * string, int position){
    char * ret;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    switch(type) {
        case ECHO:
            memcpy(toReturn, string + position + 1, strlen(string + position + 1));
            ret = toReturn;
            break;
        case GET_DATE:
            sprintf(toReturn,"%d-%02d-%02d\r\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
            ret = toReturn;
            break;
        case GET_TIME:
            sprintf(toReturn,"%02d:%02d:%02d\r\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
            ret = toReturn;
            break;
    }
    return ret;
}
static const char * event_to_action(const enum string_cmp_event_types type, char * string, int position, int * flag, const enum command_types cmd){
    const char *ret;

    switch(type) {
        case STRING_CMP_MAYEQ:
            ret = "wait(c)\r\n";
            break;
        case STRING_CMP_EQ:
            ret = call_to_action(cmd,string,position);
            (*flag) = 0;
            break;
        case STRING_CMP_NEQ:
            ret = "not a valid command.\r\n";
            (*flag)--;
            break;
    }
    return ret;
}
void init_executioner(){
    for(int i = 0; i <MAX_CMD; i++){
        to_parse[i] = TRUE;
    }

    echo_d = parser_utils_strcmpi("echo ");
    get_date_d = parser_utils_strcmpi("get date");
    get_time_d = parser_utils_strcmpi("get time");

    parsers[0] = parser_init(parser_no_classes(),&echo_d);
    parsers[1] = parser_init(parser_no_classes(),&get_date_d);
    parsers[2] = parser_init(parser_no_classes(),&get_time_d);



    printf("Init finished\n");

}

const char * execute(char * string){
    int flag;
    int valread = strlen(string);
    char * ans;
    //check if the cmd ends with \r\n
    flag = string[valread-1] == '\n' && string[valread-2] == '\r';
    if(!flag)
        ans = "not a valid cmd. \\r\\n is missing.\r\n";
    flag = MAX_CMD;
    for(int i = 0; i < valread-2 && flag; i++){
        for(int j = 0; j < MAX_CMD; j++){
            if(to_parse[j]) {
                parser_events[j] = parser_feed(parsers[j], string[i]);
                to_parse[j] = parser_events[j]->type == STRING_CMP_MAYEQ;
                //se puede hacer la logica del flag aca.
                ans = (char *) event_to_action(parser_events[j]->type, string, i, &flag, j);
            }
        }
    }
    return ans;
}

void reset_parser_executioner(){
    for(int i = 0; i<MAX_CMD; i++) {
        parser_reset(parsers[i]);
        to_parse[i] = TRUE;
    }
    for(int j = 0; j < 100 ; j++){
        toReturn[j] = 0;
    }
}
