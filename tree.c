#include "tree.h"

// Create a new node
struct node *new_node(char* name,char *path, int max_leaves, int is_local) {
  struct node *node = (struct node*) malloc(sizeof(struct node));
  strcpy(node->name, name);
  strcpy(node->path, path);
  node->leaves = (struct node**) malloc(max_leaves * sizeof(struct node*));
  node->num_leaves = 0;
  node->max_leaves = max_leaves;
  node->is_local = is_local;
  node->is_duplicate = 0;
  return node;
}

// Insert a new node into the tree
int insert_new_node(struct node *root, char* name, char *path, int max_leaves, int is_local) {
  if (root == NULL) {
    // If the tree is empty, create a new node as the root
    return 1;
  } else if (root->num_leaves < root->max_leaves) {
    // If the root node has space for more leaves, create a new leaf and insert it
    root->leaves[root->num_leaves] = new_node(name, path, max_leaves,is_local);
    root->num_leaves++;
    return 0;
  } else {
    // If the root node does not have space for more leaves, double the size of the array and insert the new node
    root->leaves = (struct node**) realloc(root->leaves,2 * root->max_leaves * sizeof(struct node*));
    root->leaves[root->num_leaves] = new_node(name, path, max_leaves,is_local);
    root->num_leaves++;
    return 2;
  }
}

// Insert an existing node into the tree
int insert_existing_node(struct node *root, struct node* node) {
  if (root == NULL) {
    printf("Failed to insert node into tree, no existing tree node found\n");
    return 1;
  } else if (root->num_leaves < root->max_leaves) {
    // If the root node has space for more leaves, create a new leaf and insert it
    root->leaves[root->num_leaves] = node;
    root->num_leaves++;
    return 0;
  } else {
    // If the root node does not have space for more leaves, double the size of the array and insert the new node
    root->leaves = (struct node**) realloc(root->leaves,2 * root->max_leaves * sizeof(struct node*));
    root->leaves[root->num_leaves] = node;
    root->num_leaves++;
    return 2;
  }
}