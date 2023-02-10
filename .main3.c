#include <stdio.h>
#include <dirent.h> //zum lesen der ordner inhalte
#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "string_helper.h"
#include "file_helper.h"
#include "dot_helper.h"
#include "make_helper.h"
#include "linked_list.h"

extern struct node **source_files;
extern struct node **main_files;
extern int num_source_files;
extern int num_main_files;
extern int max_source_files;
extern int max_main_files;


// Traverse the tree in order and print the name of each node
void in_order_traversal(FILE *dot_fp,FILE *make_fp,struct node *root, char* parent, int node) {
  //printf("parent: %s, node: %d\n", parent,node);
  if (root != NULL) {
    // Traverse the leaves of the current node
    print_make_node(make_fp,root,node);
    int i;
    for (i = 0; i < root->num_leaves; i++) {
        in_order_traversal(dot_fp,make_fp,root->leaves[i], root->name, node);
        print_dot_node(dot_fp,root->leaves[i],root);
    }
        // Print the name of the current node
        //printf("%s/%s \n", root->path, root->name);
  }
}

//checks if the file contains a main function 
int check_if_main(char * file_name, char *path){
    // Open the file
    char file_path[512];
    sprintf(file_path, "%s/%s",path,file_name);
    FILE *file = fopen(file_path, "r");

    if (file == NULL) {
        printf("Error: Could not open file '%s'\n", file_name);
        return 2;
    }
        // Read the file line by line
    char line[NAME_MAX];
    while (fgets(line, sizeof(line), file)) {
        // Check if the line contains the main function
        if (strstr(line, "int main(") != NULL || strstr(line, "void main(") != NULL) {
            if (strstr(line, ")") != NULL) {
            printf("File '%s' contains the main function\n", file_name);
                return 0;
            }
        }
    }
    return 1;
}

int file_to_node(struct dirent *dir, char *path){
    int endOfString = strlen(dir->d_name);
    if(dir->d_name[endOfString-1] != 'c'  && dir->d_name[endOfString-1] != 'h'){ //check if file does not end with c or h and skip
        return 1;
    }

    if(dir->d_name[endOfString-2] != '.'){ //check if its not the actual file ending and skip
        return 1;
    }     
    if(dir->d_name[endOfString-1] == 'c'){
        if(0 == check_if_main(dir->d_name,path)){ 
            if(num_main_files ==  max_main_files){
                max_main_files = max_main_files * 2;
                main_files = realloc(main_files,max_main_files * sizeof(struct node*));
            }
            main_files[num_main_files++] = new_node(dir->d_name, path, 5,1);
            return 0;
        }
    }
    if(num_source_files ==  max_source_files){
        max_source_files = max_source_files * 2;
        source_files = realloc(source_files,max_source_files * sizeof(struct node*));
    }
    source_files[num_source_files++] = new_node(dir->d_name, path, 5,1);
}

int main(){  
    num_source_files = 0;
    num_main_files = 0;
    max_source_files = 10;
    max_main_files = 10;  
    source_files = malloc(max_source_files * sizeof(struct node*));
    main_files = malloc(max_main_files * sizeof(struct node*));
    char **include_file_list;
    parse_directory(".");
    for(int i = 0; i<num_source_files;i++){
        //printf("source file list: name %s path %s\n", source_files[i]->name,source_files[i]->path);
        include_file_list = parse_for_include(source_files[i]->name,source_files[i]->path);
        insert_include_file_list(source_files[i],include_file_list,0);
        //printf("in order traversal source files: %s \n", source_files[i]->name);
        //in_order_traversal(source_files[i]);
    }
    FILE * dot_file, *make_file;
    int status;
    make_file = print_make_init();
    for(int i = 0; i<num_main_files;i++){    
        include_file_list = parse_for_include(main_files[i]->name,main_files[i]->path);
        
        insert_include_file_list(main_files[i],include_file_list,1);
        
        dot_file = print_dot_init(main_files[i]->name);
        if(dot_file == NULL){
            return -1;
        }
        print_make_node_init(make_file,main_files[i]->name,i);

        in_order_traversal(dot_file,make_file,main_files[i],"",i);
        fprintf(dot_file,"}\n");
        fclose(dot_file);
        exec_dot(main_files[i]->name);
        reset_duplicate();
    }
//TODO: output tree to makefile

    return 0;
}