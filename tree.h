#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

struct node {
  char name[NAME_MAX];
  char path[PATH_MAX];
  struct node **leaves;
  int num_leaves;
  int max_leaves;
  int is_local;
  int is_duplicate;
};

struct node *new_node(char* name,char *path, int max_leaves, int is_local);

int insert_new_node(struct node *root, char* name, char *path, int max_leaves, int is_local);

int insert_existing_node(struct node *root, struct node* node);