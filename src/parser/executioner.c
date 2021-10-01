#include "../../include/executioner.h"

#define BUFF_SIZE 103

static int to_parse[MAX_CMD];
static struct parser *parsers[MAX_CMD];
static struct parser_definition echo_d;
static struct parser_definition get_date_d;
static struct parser_definition get_time_d;
static char to_return[BUFF_SIZE];
static int date_format = EN;

static const char *call_to_action(const enum command_types type, char *string, int position) {
    char *ret = NULL;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int len;
    switch (type) {
        case ECHO:
            len = strlen(string + position + 1);
            memcpy(to_return, string + position + 1, len + 1);
            ret = to_return;
            break;
        case GET_DATE:
            if(date_format == ES)
                sprintf(to_return, "%02d-%02d-%d\r\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
            else
                sprintf(to_return, "%02d-%02d-%d\r\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
            ret = to_return;
            break;
        case GET_TIME:
            sprintf(to_return, "%02d:%02d:%02d\r\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
            ret = to_return;
            break;
        case SET_LOCALE_ES:
            date_format = ES;
            break;
        case SET_LOCALE_EN:
            date_format = EN;
            break;
    }
    return ret;
}

static const char *event_to_action(const enum string_cmp_event_types type, char *string, int position, int *flag,
                                   const enum command_types cmd) {
    const char *ret;

    switch (type) {
        case STRING_CMP_MAYEQ:
            ret = "Arguments missing\r\n";
            break;
        case STRING_CMP_EQ:
            ret = call_to_action(cmd, string, position);
            (*flag) = 0;
            break;
        case STRING_CMP_NEQ:
            ret = "Not a valid command.\r\n";
            (*flag)--;
            break;
    }
    return ret;
}

void init_executioner() {
    for (int i = 0; i < MAX_CMD; i++) {
        to_parse[i] = TRUE;
    }

    echo_d = parser_utils_strcmpi(CMD_ECHO);
    get_date_d = parser_utils_strcmpi(CMD_GET_DATE);
    get_time_d = parser_utils_strcmpi(CMD_GET_TIME);

    parsers[0] = parser_init(parser_no_classes(), &echo_d);
    parsers[1] = parser_init(parser_no_classes(), &get_date_d);
    parsers[2] = parser_init(parser_no_classes(), &get_time_d);

}

const char *execute(char *string) {
    int flag;
    int valread = strlen(string);
    char *ans = "";
    const struct parser_event *current_event;
    flag = MAX_CMD;
    for (int i = 0; i<valread && string[i] != '\r' && flag; i++) {
        for (int j = 0; j < MAX_CMD; j++) {
            if (to_parse[j]) {
                current_event = parser_feed(parsers[j], string[i]);
                to_parse[j] = current_event->type == STRING_CMP_MAYEQ;
                ans = (char *) event_to_action(current_event->type, string, i, &flag, j);
            }
        }
    }
    return ans;
}

void reset_parser_executioner() {
    for (int i = 0; i < MAX_CMD; i++) {
        parser_reset(parsers[i]);
        to_parse[i] = TRUE;
    }
    for (int j = 0; j < BUFF_SIZE; j++) {
        to_return[j] = 0;
    }
}
