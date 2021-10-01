#include <string.h>
#include <stdlib.h>

#include "../../include/executioner.h"
#include "../../include/tools.h"

#define BUFF_SIZE 103

static int to_parse_tcp[MAX_CMD_TCP];
static struct parser *parsers_tcp[MAX_CMD_TCP];
static int to_parse_udp[MAX_CMD_UDP];
static struct parser *parsers_udp[MAX_CMD_UDP];
static struct parser_definition echo_d;
static struct parser_definition get_date_d;
static struct parser_definition get_time_d;
static struct parser_definition set_locale_en_d;
static struct parser_definition set_locale_es_d;
static struct parser_definition stats_d;
static char to_return[BUFF_SIZE];


static const char *call_to_action(const enum command_types type, char *string, int position) {
    char *ret = NULL;
    switch (type) {
        case ECHO:
            echo(to_return,string,position);
            break;
        case GET_DATE:
            get_date(to_return);
            break;
        case GET_TIME:
            get_time(to_return);
            break;
        case SET_LOCALE_EN:
            set_locale(to_return, EN);
            break;
        case SET_LOCALE_ES:
            set_locale(to_return, ES);
            break;
        case STATS:
            stats(to_return);
            break;
    }
    ret = to_return;
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
            if(cmd < MAX_CMD_TCP) {
                post_correct_lines();
            }
            break;
        case STRING_CMP_NEQ:
            ret = "Not a valid command.Try again!\r\n";
            (*flag)--;
            break;
    }
    return ret;
}

void init_executioner() {
    for (int i = 0; i < MAX_CMD_TCP; i++) {
        to_parse_tcp[i] = TRUE;

    }
    for (int i = 0; i < MAX_CMD_UDP; i++) {
        to_parse_udp[i] = TRUE;
    }

    echo_d = parser_utils_strcmpi(CMD_ECHO);
    get_date_d = parser_utils_strcmpi(CMD_GET_DATE);
    get_time_d = parser_utils_strcmpi(CMD_GET_TIME);
    set_locale_en_d = parser_utils_strcmpi(CMD_SET_LOCALE_EN);
    set_locale_es_d = parser_utils_strcmpi(CMD_SET_LOCALE_ES);
    stats_d = parser_utils_strcmpi(CMD_STATS);

    parsers_tcp[0] = parser_init(parser_no_classes(), &echo_d);
    parsers_tcp[1] = parser_init(parser_no_classes(), &get_date_d);
    parsers_tcp[2] = parser_init(parser_no_classes(), &get_time_d);
    parsers_udp[0] = parser_init(parser_no_classes(), &set_locale_en_d);
    parsers_udp[1] = parser_init(parser_no_classes(), &set_locale_es_d);
    parsers_udp[2] = parser_init(parser_no_classes(), &stats_d);
}

const char *execute(char *string, const enum connection_type con_type){
    int flag;
    int valread = strlen(string);
    char *ans = "";
    const struct parser_event *current_event;
    int max_cmd = con_type == TCP ? MAX_CMD_TCP : MAX_CMD_UDP;
    flag = max_cmd;
    struct parser **parsers = con_type == TCP ? parsers_tcp : parsers_udp;
    int * to_parse = con_type == TCP ? to_parse_tcp : to_parse_udp;
    int offset = con_type == TCP ? 0 : MAX_CMD_TCP;
    for (int str_pos = 0; str_pos < valread && string[str_pos] != '\r' && flag; str_pos++) {
        for (int parser = 0; parser < max_cmd && flag; parser++) {
            if (to_parse[parser]) {
                current_event = parser_feed(parsers[parser], string[str_pos]);
                to_parse[parser] = current_event->type == STRING_CMP_MAYEQ;
                ans = (char *) event_to_action(current_event->type, string, str_pos, &flag, parser + offset);
            }
        }
    }
    if(current_event!=NULL && (current_event->type == STRING_CMP_NEQ || current_event->type == STRING_CMP_MAYEQ)) {
        if(con_type == TCP) {
            post_incorrect_lines();
        } else {
            post_incorrect_datagrams();
        }
    }
    return ans;
}

void reset_parser_executioner(const enum connection_type con_type) {
    int max_cmd = con_type == TCP ? MAX_CMD_TCP : MAX_CMD_UDP;
    struct parser **parsers = con_type == TCP ? parsers_tcp : parsers_udp;
    int * to_parse = con_type == TCP ? to_parse_tcp : to_parse_udp;
    for (int i = 0; i < max_cmd; i++) {
        parser_reset(parsers[i]);
        to_parse[i] = TRUE;
    }
    for (int j = 0; j < BUFF_SIZE; j++) {
        to_return[j] = 0;
    }
}

void destroy_executioner() {
    for(int i = 0; i < MAX_CMD_TCP; i++) {
        parser_destroy(parsers_tcp[i]);
    }
    for(int i = 0; i < MAX_CMD_UDP; i++) {
        parser_destroy(parsers_udp[i]);
    }
}
