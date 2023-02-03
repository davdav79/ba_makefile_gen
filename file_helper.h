#pragma once
#include <stdio.h>
#include <dirent.h> //zum lesen der ordner inhalte
#include <stdlib.h>
#include "tree.h"
#include "string_helper.h"


struct node **source_files;
struct node **main_files;
int num_source_files;
int num_main_files;
int max_source_files;
int max_main_files;

void jump_to_line(FILE * fp, int line_number);

void jump_to_eol(FILE *fp, int line_number);

char **parse_for_include(char *file_name, char *path);

struct node *find_file(char *path, char *file_name);

void insert_include_file_list(struct node *root, char **files, int is_main);

int parse_directory(char *path);

void reset_duplicate();

int file_to_node(struct dirent *dir, char *path);

int check_if_main(char * file_name, char *path);