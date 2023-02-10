#pragma once
#include <stdio.h>
#include <dirent.h> //zum lesen der ordner inhalte
#include <stdlib.h>
#include "tree.h"
#include "string_helper.h"
#include "linked_list.h"
#include <limits.h>
#include "pthread.h"
#include "math.h"

struct list_node *source_files;
struct list_node *main_files;

void jump_to_line(FILE * fp, int line_number);

void jump_to_eol(FILE *fp, int line_number);

void parse_for_insert_and_create_node(struct node** root, struct list_node **source_files);

struct node *find_file(char* name, struct list_node *list);

void insert_and_append_node(struct list_node **list,struct node* root, char* path_name);

int parse_directory(char* path, struct list_node ** source_files);

void reset_duplicate(struct list_node *list);

int check_if_main(char * file_name, char *path);

int file_to_node(struct dirent *dir, char *path, struct list_node **source_files);
