#include <stdio.h>
#include <dirent.h> //zum lesen der ordner inhalte
#include <string.h>
#include <stdlib.h>
#include <ctype.h> //isspace check if character is a whitespace
#include "test1.h"

#define MAX_PATH_LEN 1024
struct node **source_files;
struct node **main_files;
int num_source_files = 0,num_main_files = 0;
int max_source_files = 10,max_main_files = 10;

struct node {
  char data[256];
  struct node **leaves;
  int num_leaves;
  int max_leaves;
};

// Create a new node
struct node *new_node(char* data, int max_leaves) {
  struct node *node = (struct node*) malloc(sizeof(struct node));
  strcpy(node->data, data);
  node->leaves = (struct node**) malloc(max_leaves * sizeof(struct node*));
  node->num_leaves = 0;
  node->max_leaves = max_leaves;
  return node;
}

// Insert a new node into the tree
int insert_new_node(struct node *root, char* data, int max_leaves) {
  if (root == NULL) {
    // If the tree is empty, create a new node as the root
    return 1;
  } else if (root->num_leaves < root->max_leaves) {
    // If the root node has space for more leaves, create a new leaf and insert it
    root->leaves[root->num_leaves] = new_node(data, max_leaves);
    root->num_leaves++;
    return 0;
  } else {
    // If the root node does not have space for more leaves, double the size of the array and insert the new node
    root->leaves = (struct node**) realloc(root->leaves,2 * root->max_leaves * sizeof(struct node*));
    root->leaves[root->num_leaves] = new_node(data, max_leaves);
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

// Traverse the tree in order and print the data of each node
void in_order_traversal(struct node *root) {
  if (root != NULL) {
    // Traverse the leaves of the current node
    int i;
    for (i = 0; i < root->num_leaves; i++) {
        in_order_traversal(root->leaves[i]);
    }
        // Print the data of the current node
        printf("%s \n", root->data);
  }
}

int whitespace_check(char* line_start, char* found_location){
    int offset = found_location - line_start -1;
    while(offset >= 0){
        if(!isspace(line_start[offset]))
            return 1;
        offset--;
    }
    return 0;
}

char** parse_for_include(char *filename){
    FILE *file = fopen(filename, "r");
    int file_count = 0, max_files = 1;
    char **files = malloc(max_files * sizeof(char *));
    printf("parse_for_include %s\n",filename);
    if (file == NULL) {
        printf("Error: Could not open file '%s'\n", filename);
        return NULL;
    }
    // Read the file line by line
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Check if the line is an include statement
        char *include_start = strstr(line, "#include");
        if (include_start == NULL)
            continue;
        //check if #include is on the beginning of the line or has leading whitespaces
        //http://public.beuth-hochschule.de/~kempfer/script_c/c.pdf seite 86/87
        if((void*)&line != (void*)include_start) {
            if(whitespace_check(line,include_start) == 1)
                continue;
        }
        // Extract the include file name from the line
        char *include_file = strstr(line, "<");
        char *include_file_end = NULL;
        if(include_file != NULL){
            include_file_end = strstr(line, ">");
        }
        else{
            include_file = strstr(line, "\"");
            include_file_end = strstr(include_file+1, "\"");
        }
        int include_file_len = include_file_end - include_file-1;
        char include_file_name[include_file_len];
        strncpy(include_file_name, include_file+1, include_file_len);
        include_file_name[include_file_len] = '\0';

        // Print the include file name
        //printf("Include file: %s\n", include_file_name);
        if(file_count ==  max_files){
                max_files = max_files * 2;
                files = realloc(files,max_files * sizeof(struct node*));
        } 
        files[file_count++] = malloc(256*sizeof(char));
        //printf("malloc\n");
        strcpy(files[file_count-1], include_file_name);
    }
    if(file_count ==  max_files){
                max_files = max_files * 2;
                files = realloc(files,max_files * sizeof(struct node*));
    } 
    files[file_count] = malloc(256*sizeof(char));
    files[file_count] = 0;
    return files;
}

struct node * find_file(char* file_name){
    if(file_name[strlen(file_name)-1] =='h'){
        for(int i = 0; i < num_source_files;i++){
            if(0 == strcmp(source_files[i]->data,file_name))
                return source_files[i];
        }
    }else{
        for(int i = 0; i < num_main_files;i++){
            if(0 == strcmp(main_files[i]->data,file_name))
                return main_files[i];
        }
    }
    return NULL;
}
void insert_include_file_list(struct node *root, char ** files, int is_main){
    int i = 0, j = 0;
    struct node * include_file = NULL;
    while(files[i] != 0){
        printf("insert include files: %s\n",files[i]);
        include_file = find_file(files[i]);
        if(include_file){
            printf("insert existing node %c %d\n",(root->data[strlen(root->data)-1]) ,(root->data[strlen(root->data)-1]) == 'c');
            if('c' == (root->data[strlen(root->data)-1]) && 0 == is_main){
                printf("not main c file\n");
                insert_existing_node(include_file,root);
            }else{
                printf("main c file\n");
                insert_existing_node(root,include_file);
            }
        }
        else{
            printf("insert new node\n");
            insert_new_node(root,files[i],5);
        }
        free(files[i]);
        i++;
    }
    free(files[i]);
    free(files);   
}

//checks if the file contains a main function 
int check_if_main(char * file_name){
    // Open the file
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        printf("Error: Could not open file '%s'\n", file_name);
        return 2;
    }
        // Read the file line by line
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Check if the line contains the main function
        if (strstr(line, "int main") != NULL || strstr(line, "void main") != NULL) {
        printf("File '%s' contains the main function\n", file_name);
        return 0;
        }
    }
    return 1;
}

int main(){
    DIR *d;
    struct dirent *dir;
    
    source_files = malloc(max_source_files * sizeof(struct node*));
    main_files = malloc(max_main_files * sizeof(struct node*));
    char **include_file_list;
    d = opendir(".");
    if (!d) {
        printf("could not open the directory: \n");
        return 1;
    }
//TODO: add files from folders    
    while ((dir = readdir(d)) != NULL) {
        //printf("%s,%d\n",dir->d_name,dir->d_type);
        int endOfString = strlen(dir->d_name);
        if(dir->d_name[endOfString-1] != 'c'  && dir->d_name[endOfString-1] != 'h'){ //check if file does not end with c or h and skip
            continue;
        }

        if(dir->d_name[endOfString-2] != '.'){ //check if its not the actual file ending and skip
            continue;
        }     
        if(dir->d_name[endOfString-1] == 'c'){
            if(0 == check_if_main(dir->d_name)){ 
                if(num_main_files ==  max_main_files){
                    max_main_files = max_main_files * 2;
                    main_files = realloc(main_files,max_main_files * sizeof(struct node*));
                }
                main_files[num_main_files++] = new_node(dir->d_name,5);
                printf("main file: %s\n",main_files[num_main_files-1]->data);
                continue;
            }
        }
        if(num_source_files ==  max_source_files){
            max_source_files = max_source_files * 2;
            source_files = realloc(source_files,max_source_files * sizeof(struct node*));
        }
        source_files[num_source_files++] = new_node(dir->d_name,5);
        printf("source file: %s\n",source_files[num_source_files-1]->data);
    }
    for(int i = 0; i<num_source_files;i++){
        //printf("source file list: %s\n", source_files[i]->data);
        include_file_list = parse_for_include(source_files[i]->data);
        insert_include_file_list(source_files[i],include_file_list,0);
        printf("in order traversal: %s \n", source_files[i]->data);
        in_order_traversal(source_files[i]);
    }
    for(int i = 0; i<num_main_files;i++){
        include_file_list = parse_for_include(main_files[i]->data);
        
        insert_include_file_list(main_files[i],include_file_list,1);
        printf("in order traversal:\n");
        in_order_traversal(main_files[i]);
    }
//TODO: output tree to dot
//TODO generate make file
//TODO: output tree to makefile

    closedir(d);

    return 0;
}