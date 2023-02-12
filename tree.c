#include "tree.h"

// Create a new node
struct node *new_node(char* name,char *path, bool is_local, bool is_main) {
  struct node *node = (struct node*) malloc(sizeof(struct node));
  strcpy(node->name, name);
  strcpy(node->path, path);
  node->leaves = NULL;
  node->is_local = is_local;
  node->is_main = is_main;
  node->is_duplicate = false;
  return node;
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
void make_list_node(struct list_node *make_list, struct node *node);
void print_dot_node(FILE *fp, struct node *node, struct node *parent);
// Traverse the tree in order and print the name of each node
void in_order_traversal(FILE *dot_fp,struct list_node *make_list,struct node *root) {
  if (root != NULL) {
    // Traverse the leaves of the current node
    make_list_node(make_list,root);
    struct list_node * leaf = ((struct list_node *)root->leaves);
    while (NULL != leaf ) {
        in_order_traversal(dot_fp,make_list,leaf->data);
        print_dot_node(dot_fp,((struct node *)leaf->data),root);
        leaf = leaf->next;  
    }
  }
}