#include "string_helper.h"

void to_upper(char *str) {
    int i = 0;
    while (str[i]) {
        str[i] = toupper(str[i]);
        i++;
    }
}
void to_lower(char *str) {
    int i = 0;
    while (str[i]) {
        str[i] = tolower(str[i]);
        i++;
    }
}

int whitespace_check(char* line_start, char* found_location){
    int offset = found_location - line_start -1;
    while(offset >= 0){
        if(!isspace(line_start[offset]))
            return 1;
        offset--;
    }
    return 0;
}