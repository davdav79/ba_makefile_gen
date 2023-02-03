#pragma once
#include <ctype.h> //isspace check if character is a whitespace

void to_lower(char *str);
void to_upper(char *str);
int whitespace_check(char *line_start, char *found_location);
