#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include "linked_list.h"


struct node {
  char name[NAME_MAX];
  char path[PATH_MAX];
  struct list_node *leaves;
  bool is_main;
  bool is_local;
  bool is_duplicate;
};

struct node *new_node(char* name,char *path, bool is_local, bool is_main);

int append_existing_node(struct node *head, struct node* node);

void in_order_traversal(FILE *dot_fp,struct list_node *make_list,struct node *root);