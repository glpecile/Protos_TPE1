#include "../../include/executioner.h"

#define BUFF_SIZE 103

static int to_parse[MAX_CMD];
static struct parser *parsers[MAX_CMD];
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
            printf("queremos cambiar el locale\r\n");
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
            break;
        case STRING_CMP_NEQ:
            ret = "Not a valid pepe.\r\n";
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
    set_locale_en_d = parser_utils_strcmpi(CMD_SET_LOCALE_EN);
    set_locale_es_d = parser_utils_strcmpi(CMD_SET_LOCALE_ES);
    stats_d = parser_utils_strcmpi(CMD_STATS);

    parsers[0] = parser_init(parser_no_classes(), &echo_d);
    parsers[1] = parser_init(parser_no_classes(), &get_date_d);
    parsers[2] = parser_init(parser_no_classes(), &get_time_d);
    parsers[3] = parser_init(parser_no_classes(), &set_locale_en_d);
    parsers[4] = parser_init(parser_no_classes(), &set_locale_es_d);
    parsers[5] = parser_init(parser_no_classes(), &stats_d);
}

const char *execute(char *string) {
    int flag;
    int valread = strlen(string);
    char *ans = "";
    const struct parser_event *current_event;
    flag = MAX_CMD;
    for (int i = 0; i<valread && string[i] != '\r' && flag; i++) {
        for (int j = 0; j < MAX_CMD && flag; j++) {
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
