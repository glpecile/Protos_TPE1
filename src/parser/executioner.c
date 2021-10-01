#include "../../include/executioner.h"

#define BUFF_SIZE 103

static int to_parse[MAX_CMD];
static struct parser *parsers[MAX_CMD];
static struct parser_definition echo_d;
static struct parser_definition get_date_d;
static struct parser_definition get_time_d;
static char toReturn[BUFF_SIZE];

static int check_ascii_validation(const unsigned char *string, int len) {
    for (int i = 0; i < len; i++) {
        if (string[i] > US_ASCII_LIMIT)
            return FALSE;
        printf("char: %d\n", string[i]);
    }
    return TRUE;
}

static const char *call_to_action(const enum command_types type, char *string, int position) {
    char *ret;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int len;
    switch (type) {
        case ECHO:
            len = strlen(string + position + 1);
            printf("len: %d\n", len);
            if (check_ascii_validation((unsigned char *) (string + position + 1), len)) {
                memcpy(toReturn, string + position + 1, len + 1);
                ret = toReturn;
            } else {
                ret = "not a valid command. Only US-ASCII characters.";
            }
            break;
        case GET_DATE:
            sprintf(toReturn, "%02d-%02d-%d\r\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
            ret = toReturn;
            break;
        case GET_TIME:
            sprintf(toReturn, "%02d:%02d:%02d\r\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
            ret = toReturn;
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
    //check if the cmd ends with \r\n
    flag = string[valread - 1] == '\n' && string[valread - 2] == '\r';
    if (!flag)
        ans = "Not a valid command. \\r\\n is missing.\r\n";
    flag = MAX_CMD;
    for (int i = 0; string[i] != '\r' && flag; i++) {
        for (int j = 0; j < MAX_CMD; j++) {
            if (to_parse[j]) {
                if (string[i] <= US_ASCII_LIMIT) {
                    current_event = parser_feed(parsers[j], string[i]);
                    to_parse[j] = current_event->type == STRING_CMP_MAYEQ;
                    //se puede hacer la logica del flag aca.
                    ans = (char *) event_to_action(current_event->type, string, i, &flag, j);
                } else {
                    ans = "Not a valid command. Only US-ASCII characters.";
                    flag = FALSE;
                    break;
                }
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
        toReturn[j] = 0;
    }
}
