#include "tree.h"

// Create a new node
struct node *new_node(char* name,char *path, int is_local) {
  struct node *node = (struct node*) malloc(sizeof(struct node));
  strcpy(node->name, name);
  strcpy(node->path, path);
  node->leaves = NULL;
  node->is_local = is_local;
  node->is_duplicate = 0;
  return node;
}

// Insert a new node into the tree
int insert_new_node(struct node *head, char* name, char *path, int is_local) {
  if (head == NULL) {
    fprintf(stderr,"Failed to insert node into tree, no existing tree node found\n");
    return 1;
  } 
  //printf("insert new node: %s/%s\n", path,name);
  list_insert_node(&head->leaves,new_node(name, path, is_local));
  return 0;
}

// Insert an existing node into the tree
int append_existing_node(struct node *head, struct node* node) {
  if (head == NULL) {
    fprintf(stderr,"Failed to insert node into tree, no existing tree node found\n");
    return 1;
  }
  list_insert_node(&head->leaves,node);
  return 0;
}