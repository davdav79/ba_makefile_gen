#pragma once
#include <stdio.h>
#include <stdlib.h>

struct list_node{
    void * data;
    struct list_node *next;
};
void list_insert_node(struct list_node **head, void*data);
void list_remove_node(struct list_node **head);
struct list_node *list_get_last_node(struct list_node *head);
