#include "dot_helper.h"


void print_dot_node(FILE *fp, struct node *node, struct node *parent){
    if(parent->is_duplicate)
        return;
    fprintf(fp, "\t\"%s/%s\" -> \"%s/%s\";\n",parent->path,parent->name,node->path,node->name);
    node->is_duplicate = 1;
}

FILE* print_dot_init(char *file_name){
    char dot_file_name[NAME_MAX];
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
    char tmp_file_name[NAME_MAX];
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