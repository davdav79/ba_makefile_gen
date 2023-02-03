#include "linked_list.h"

void list_insert_node(struct make_node **head,struct node *node) {
   struct make_node *temp = new_make_node(node);
   
   temp->next = *head;
   *head = temp;
}

void list_insert_list(struct list_node **head) {
   struct list_node *temp = new_list_node();
   temp->next = *head;
   *head = temp;
}

struct make_node *new_make_node(struct node *node) {
  struct make_node *_node = (struct make_node*) malloc(sizeof(struct make_node));
  _node->next = NULL;
  _node->node = node;
  return _node;
}

struct list_node *new_list_node() {
  struct list_node *node = (struct list_node*) malloc(sizeof(struct list_node));
  node->next = NULL;
  node->node = NULL;
  return node;
}

struct make_node *list_get_last_node(struct make_node *head){
   while(NULL != head->next) {
      head = head->next;
   }
   return head;
}
