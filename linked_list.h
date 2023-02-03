#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

struct make_node {
   struct node *node;
   struct make_node *next;
};

struct list_node {
    struct make_node *node;
    struct list_node *next;    
};

void list_insert_node(struct make_node **head,struct node *node);
void list_insert_list(struct list_node **head);
struct make_node *new_make_node(struct node *node);
struct list_node *new_list_node();
struct make_node *list_get_last_node(struct make_node *head);