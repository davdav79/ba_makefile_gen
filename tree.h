#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "linked_list.h"

struct node {
  char name[NAME_MAX];
  char path[PATH_MAX];
  struct list_node *leaves;
  int is_local;
  int is_duplicate;
};

struct node *new_node(char* name,char *path, int is_local);

int insert_new_node(struct node *head, char* name, char *path, int is_local);

int append_existing_node(struct node *head, struct node* node);

void link_node(struct node*head, struct node* node);