#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h> //FILE struct
#include "tree.h"
#include "string_helper.h"
#include "file_helper.h"
#include "linked_list.h"

int dotO;

//void print_make_node_init(struct list_list *make_list, char *name_ex);
void make_list_node(struct list_node *make_list, struct node *node);
void print_make(FILE *fp,struct list_node *make_list);
FILE *make_init();
