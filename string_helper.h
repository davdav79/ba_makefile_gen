#pragma once
#include <ctype.h> //isspace check if character is a whitespace

void to_lower(char *str);
void to_upper(char *str);
int whitespace_check(char *line_start, char *found_location);
int check_if_comment(char last_char,char line,int *block_comment);
int check_if_string_literal(char last_char, char line, int *string_double, int *string_single );