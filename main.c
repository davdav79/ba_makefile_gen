#include <stdio.h>
#include <dirent.h> //zum lesen der ordner inhalte
#include <string.h>
#include <stdlib.h>
#include <ctype.h> //isspace check if character is a whitespace
#include "test1.h"
#include "./test/test1-Copy.h"

#define MAX_PATH_LEN 1024
struct node **source_files;
struct node **main_files;
int num_source_files = 0,num_main_files = 0;
int max_source_files = 10,max_main_files = 10;

struct node {
  char name[256];
  char path[256];
  struct node **leaves;
  int num_leaves;
  int max_leaves;
  int is_local;
};

// Create a new node
struct node *new_node(char* name,char *path, int max_leaves, int is_local) {
  struct node *node = (struct node*) malloc(sizeof(struct node));
  strcpy(node->name, name);
  strcpy(node->path, path);
  node->leaves = (struct node**) malloc(max_leaves * sizeof(struct node*));
  node->num_leaves = 0;
  node->max_leaves = max_leaves;
  node->is_local = is_local;
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

FILE* print_make_init(){
    char make_file_name[10];
    sprintf(make_file_name, "Makefile");
    FILE* fp = fopen(make_file_name, "w+");
    if (fp == NULL) {
        fprintf(stderr, "Error: Unable to create file '%s'\n", make_file_name);
        return NULL;
    } 
    fprintf(fp, ".DEFAULT_GOAL := all\n.PHONY: all\nCC=gcc\n\nall:");  
    return fp;
}
void jump_to_line(FILE *fp, int line_number) {
    fseek(fp, 0, SEEK_SET);
    char buffer[1000];
    int current_line = 1;
    long pos = ftell(fp); // get the current position of the file pointer
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (current_line == line_number) {
            fseek(fp, pos, SEEK_SET);
            break;
        }
        current_line++;
        pos = ftell(fp);
    }
}
void jump_to_eol(FILE *fp, int line_number) {
    fseek(fp, 0, SEEK_SET);
    char buffer[1000];
    int current_line = 1;
    long pos = ftell(fp); // get the current position of the file pointer
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        pos = ftell(fp);
        if (current_line == line_number) {
            fseek(fp, pos-1, SEEK_SET);
            break;
        }
        current_line++;
    }
}
void to_upper(char *str) {
    int i = 0;
    while (str[i]) {
        str[i] = toupper(str[i]);
        i++;
    }
}
void to_lower(char *str) {
    int i = 0;
    while (str[i]) {
        str[i] = tolower(str[i]);
        i++;
    }
}

void print_make_node_init(FILE *fp,char *name_ex, int offset){
    fseek(fp,0,SEEK_END);
    char name_up[256],name_low[256];
    sprintf(name_up, "%s",name_ex);
    char *p;
    p = strchr(name_up, '.');
    if (p != NULL) {
        *p = 0;
    }
    sprintf(name_low, "%s",name_up);
    to_upper(name_up);
    to_lower(name_low);
    fprintf(fp," %s\n",name_low);
    long eof = ftell(fp);

    jump_to_line(fp,5+8*offset); //line 5 is where the nodes start nodes are 8 lines long so 5 + 8*nodes
    //printf("offset: %d eof:%ld malloc size: %ld, fp: %ld\n",offset,eof, eof - ftell(fp),ftell(fp));
    char * tmp = malloc((eof-ftell(fp)-1)*sizeof(char));
    fread(tmp,eof-ftell(fp)-1,sizeof(char),fp);
    jump_to_line(fp,5+8*offset);
    //printf("sof\n%s eof fp: %ld\n",tmp,ftell(fp));
    fprintf(fp,"####################%s####################\n%sCFLAGS=\n%sDEPS=\n%sOBJ=\n\n%s: $(%sOBJ) $(%sDEPS)\n\t$(CC) -o %s $(%sOBJ) $(%sCFLAGS)\n\n",name_up,name_up,name_up,name_up,name_low,name_up,name_up,name_up,name_up,name_up);
    printf("%s\n",tmp);
    fprintf(fp, "%s",tmp);
}
void print_make_node(FILE *fp, struct node *node,int offset){
    long eof = ftell(fp);
    fseek(fp,0,SEEK_END);
    int flag = 0;
    int new_offset;
    if(0 == node->is_local)
        return;
    if(flag) {
        jump_to_eol(fp,6+8*offset); //line 6 is where the flags start nodes are 8 lines long so 6 + 8*nodes 
    }
    if(node->name[strlen(node->name)-1] == 'c'){ //if the file is a .c it gets appended to obj
        new_offset =8+8*offset;
        printf("c file %s\n", node->name);
    }else{ //if the file is a .h it gets appended to deps
        new_offset= 7+8*offset;
        //printf("h file %s\n", node->name);
    }
    //printf("path: %s\n",node->path);
    jump_to_eol(fp,new_offset);
    //printf("offset: %d eof:%ld malloc size: %ld, fp: %ld\n",offset,eof, eof - ftell(fp),ftell(fp));
    char * tmp = malloc((eof-ftell(fp))*sizeof(char));
    fread(tmp,eof-ftell(fp),sizeof(char),fp);
    jump_to_eol(fp,new_offset);
    fprintf(fp," %s/%s",node->path,node->name);
    fprintf(fp, "%s",tmp);
}
void print_dot_node(FILE *fp,char *name, char *path, char * parent_name, char *parent_path){
    fprintf(fp, "\t\"%s/%s\" -> \"%s/%s\";\n",parent_path,parent_name,path,name);
}

FILE* print_dot_init(char *file_name){
    char dot_file_name[256];
    sprintf(dot_file_name, "%s",file_name);
    char *p;
    p = strchr(dot_file_name, '.');
    if (p != NULL) {
        *p = 0;
    }
    sprintf(dot_file_name,"%s.dot",dot_file_name);
    FILE* fp = fopen(dot_file_name, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: Unable to create file '%s'\n", dot_file_name);
        return NULL;
    } 
    fprintf(fp, "digraph {\n");  
    return fp;
}

void exec_dot(char *file_name){
    //get file name without extension
    char tmp_file_name[256];
    sprintf(tmp_file_name, "%s",file_name);
    char *dot;
    dot = strchr(tmp_file_name, '.');
    if (dot != NULL) {
        *dot = 0;
    }
    
    int status;
    char cmd[256];
    sprintf(cmd,"dot -Tpng %s.dot -o %s.png", tmp_file_name,tmp_file_name);
    printf("%s\n",cmd);
    status = system(cmd);
    if (status != 0) {
        fprintf(stderr, "Error: Unable to render tree\n");
    }
}
// Traverse the tree in order and print the name of each node
void in_order_traversal(FILE *dot_fp,FILE *make_fp,struct node *root, char* parent, int node) {
  //printf("parent: %s, node: %d\n", parent,node);
  if (root != NULL) {
    // Traverse the leaves of the current node
    print_make_node(make_fp,root,node);
    int i;
    for (i = 0; i < root->num_leaves; i++) {
        in_order_traversal(dot_fp,make_fp,root->leaves[i], root->name, node);
        print_dot_node(dot_fp,root->leaves[i]->name,root->leaves[i]->path,root->name,root->path);
    }
        // Print the name of the current node
        //printf("%s/%s \n", root->path, root->name);
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

//finds all #include <...> annd #include"..."  and if it doesent exist, adds it to the file list.
char** parse_for_include(char *file_name, char *path){ 
    char file_path[512];
    sprintf(file_path, "%s/%s",path,file_name);
    FILE *file = fopen(file_path, "r");
    int file_count = 0, max_files = 1;
    char **files = malloc(max_files * sizeof(char *));
    if (file == NULL) {
        printf("Error: Could not open the file '%s'\n", file_path);
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

        if(file_count ==  max_files){
                max_files = max_files * 2;
                files = realloc(files,max_files * sizeof(struct node*));
        } 
        files[file_count++] = malloc(256*sizeof(char));
        strcpy(files[file_count-1], include_file_name);
        //printf("include files %s/%s: %s\n",path,file_name,files[file_count-1]);
    }
    // add an empty node to indicate the end of the list
    if(file_count ==  max_files){
                max_files = max_files * 2;
                files = realloc(files,max_files * sizeof(struct node*));
    } 
    files[file_count] = malloc(256*sizeof(char));
    files[file_count] = 0;
    return files;
}

struct node * find_file(char* path, char* file_name){
    printf("find file: %s_%s\n",path,file_name);
    for(int i = 0; i < num_source_files;i++){
        if(0 == strcmp(source_files[i]->name,file_name)){
            if(0 == strcmp(source_files[i]->path,path)){
                return source_files[i];
            }
        }
    }
    return NULL;
}
void insert_include_file_list(struct node *root, char ** files, int is_main){
    int i = 0, j = 0;
    struct node * include_file = NULL;
    char * delim = NULL;
    while(files[i] != 0){
        delim = strrchr(files[i],'/');
        if(delim){
            files[i][delim-files[i]] = '\0';
            printf("delim: path:%s name:%s\n",delim, files[i]);
            if('.' == files[i][0]){
                include_file = find_file(files[i], delim+1);
            }else{
                char path[512];
                sprintf(path, "%s/%s",root->path,files[i]);
                include_file = find_file(path, delim+1);
            }
            
        }else{
            include_file = find_file(root->path,files[i]);
        }
        if(NULL != include_file){
            printf("insert existing node c\n");
            if('c' == (root->name[strlen(root->name)-1]) && 0 == is_main){
                insert_existing_node(include_file,root);
            }else{
                insert_existing_node(root,include_file);
            }
        }
        else{
            insert_new_node(root,files[i],".",5,0);
        }
        printf("free files[i]: %s\n",files[i]);
        free(files[i]);
        i++;
    }
    free(files[i]);
    free(files);   
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
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Check if the line contains the main function
        if (strstr(line, "int main") != NULL || strstr(line, "void main") != NULL) {
        //printf("File '%s' contains the main function\n", file_name);
        return 0;
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
            //printf("main file: %s\n",main_files[num_main_files-1]->name);
            return 0;
        }
    }
    if(num_source_files ==  max_source_files){
        max_source_files = max_source_files * 2;
        source_files = realloc(source_files,max_source_files * sizeof(struct node*));
    }
    source_files[num_source_files++] = new_node(dir->d_name, path, 5,1);
}

int parse_directory(char* path){
    DIR *d;
    struct dirent *dir;
    //printf("parse directory: %s\n", path);
    d = opendir(path);
    if (!d) {
        printf("could not open the directory: \n");
        return -1;
    }
//TODO: add files from folders    
    while ((dir = readdir(d)) != NULL) {
        //printf("%s,%d\n",dir->d_name,dir->d_type);
        if(dir->d_name[0] == '.')//to not get an infinite loop where we look into the current directory
            continue;
        if(dir->d_type == 4){   //d_type 4 = directory
            char new_path[512];
            sprintf(new_path,"%s/%s",path,dir->d_name);
            parse_directory(new_path);
        }else{
            file_to_node(dir,path);
        }
        
    }
    closedir(d);
}
int main(){    
    source_files = malloc(max_source_files * sizeof(struct node*));
    main_files = malloc(max_main_files * sizeof(struct node*));
    char **include_file_list;
    parse_directory(".");
    for(int i = 0; i<num_source_files;i++){
        printf("source file list: name %s path %s\n", source_files[i]->name,source_files[i]->path);
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
        printf("%s\n",main_files[i]->name);
        print_make_node_init(make_file,main_files[i]->name,i);

        in_order_traversal(dot_file,make_file,main_files[i],"",i);
        fprintf(dot_file,"}\n");
        fclose(dot_file);
        exec_dot(main_files[i]->name);

        
    }
//TODO: output tree to makefile

    return 0;
}