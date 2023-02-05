#include <stdio.h>
#include <dirent.h> //zum lesen der ordner inhalte
#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include <limits.h>
#include "string_helper.h"
#include "file_helper.h"
#include "dot_helper.h"
#include "make_helper.h"
#include "linked_list.h"

extern int dotO;


int file_to_node(struct dirent *dir, char *path, struct list_node **source_files,struct list_node **main_files){
    int endOfString = strlen(dir->d_name);
    if(dir->d_name[endOfString-1] != 'c'  && dir->d_name[endOfString-1] != 'h'){ //check if file does not end with c or h and skip
        return 1;
    }

    if(dir->d_name[endOfString-2] != '.'){ //check if its not the actual file ending and skip
        return 1;
    }     
    if(dir->d_name[0] == '.' ){ //ignore hidden files
        return 1;
    }
    if(dir->d_name[endOfString-1] == 'c'){
        if(0 == check_if_main(dir->d_name,path)){
            list_insert_node(main_files,(void*)new_node(dir->d_name, path, 1));
            return 0;
        }
    }
    list_insert_node(source_files,new_node(dir->d_name, path, 1));
    return 0;
}

//checks if the file contains a main function 
int check_if_main(char * file_name, char *path){
    // Open the file
    char file_path[PATH_MAX+NAME_MAX];
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
        //if(strstr(line,"\"int main(\"") == NULL || strstr(line,"\"int void(\"") ==NULL )
        //    return 1;
        //TODO check for comment before int main    
        if (strstr(line, "int main(") != NULL || strstr(line, "void main(") != NULL) {
            if (strstr(line, ")") != NULL) {
            printf("File '%s' contains the main function\n", file_name);
                return 0;
            }
        }
    }
    return 1;
}

int comment_check(FILE* fp){
    (void*)fp;
    return 0;
}

// Traverse the tree in order and print the name of each node
void in_order_traversal(FILE *dot_fp,struct list_node *make_list,struct node *root) {
  if (root != NULL) {
    // Traverse the leaves of the current node
    make_list_node(make_list,root);
    //printf("root: %s pntr:%ld\n",root->name, root);
    struct list_node * leaf = ((struct list_node *)root->leaves);
    while (NULL != leaf ) {
        //printf("parend: %s %ld child: %s %ld\n", root->name, root, ((struct node*)leaf->data)->name, leaf);
        in_order_traversal(dot_fp,make_list,leaf->data);
        print_dot_node(dot_fp,((struct node *)leaf->data),root);
        leaf = leaf->next;  
    }
  }
}

int main(){   
    dotO = 1;
    struct list_node *source_files = NULL;
    struct list_node *main_files = NULL;
    parse_directory(".",&source_files,&main_files);
    struct list_node *tmp_source_files = source_files;
    while(tmp_source_files != NULL){
        parse_and_create_node(((struct node*)tmp_source_files->data),&source_files,0);
        tmp_source_files = tmp_source_files->next;
    }
    tmp_source_files = source_files;
    printf("before files in src files\n");
    /*while(tmp_source_files){
        printf("files in source files: %s\n",((struct node*)tmp_source_files->data)->name);
        struct list_node* leaf = (struct list_node*)((struct node*)tmp_source_files->data)->leaves;
        while(leaf){
            printf("\tchild: %s\n", ((struct node*)leaf->data)->name);
            leaf =leaf->next;
        }
        tmp_source_files = tmp_source_files->next;
    }*/

    FILE * dot_file, *make_file;
    struct list_node *make_list = NULL;
    make_file = make_init();
    struct list_node *tmp_main_files = main_files;
    while(NULL != tmp_main_files){
        list_insert_node(&make_list,NULL);
        list_insert_node(&make_list,NULL);
        printf("main file: %s\n", ((struct node*)tmp_main_files->data)->name); 
        parse_and_create_node(((struct node*)tmp_main_files->data),&source_files,1);
        dot_file = print_dot_init(((struct node*)tmp_main_files->data)->name);
        if(dot_file == NULL){
            return -1;
        }
        printf("iot\n");
        in_order_traversal(dot_file,make_list,((struct node*)tmp_main_files->data));
        //printf("%s\n",((struct node*)tmp_main_files->data)->name);
        fprintf(dot_file,"}\n");
        fclose(dot_file);
        exec_dot(((struct node*)tmp_main_files->data)->name);
        reset_duplicate(source_files);
        tmp_main_files = tmp_main_files->next;
    }
    
        print_make(make_file,make_list);

    return 0;
}