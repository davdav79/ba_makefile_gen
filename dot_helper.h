#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h> //FILE struct
#include "tree.h"

void exec_dot(char *file_name);
FILE *print_dot_init(char *file_name);
void print_dot_node(FILE *fp, struct node *node, struct node *parent);
