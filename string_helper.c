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

int check_if_comment(char last_char,char line,int *block_comment){
    if(1 == *block_comment){
        if('*' == last_char && '/' == line){
            *block_comment = 0;
            return 1;
        }else{
            return 1;
        }
    }
    if('/' == last_char){
        if('/' == line){
            last_char = '\0';
            return 2;
        }else if('*' == line){
            *block_comment = 1;
        }
    }
    return 0;
}
int check_if_string_literal(char last_char, char line, int *string_double, int *string_single ){
    if('\\' != last_char){
        if(0 == *string_double){
            if('\"' == line){
                *string_double = 1;
                return 1;
            }
            if('\'' == line){
                *string_single = 1;
                return 1;
            }
        }else{
            if('\"' == line){
                *string_double = 0;
            }
            if('\'' == line){
                *string_single = 0;
            }
        return 1;
        }
    }
    return 0;
}