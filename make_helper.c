#include "make_helper.h"

FILE *make_init(){
    FILE* fp = fopen("Makefile_", "w+");
    if (fp == NULL) {
        perror("Error: ");
        return NULL;
    }
    return fp;
}

void list_make_node(struct list_node *make_list, struct node *node){
    if(node->is_duplicate)
        return;
    if(0 == node->is_local)
        return;
    if(node->name[strlen(node->name)-1] == 'c'){ //if the file is a .c it gets appended to obj
        list_insert_node(&make_list->node,node);
        
    }else{ //if the file is a .h it gets appended to deps
        list_insert_node(&make_list->next->node,node);
        
    }
}

void print_make_header(FILE *fp,struct list_node *make_list){
    struct make_node *make_tmp = NULL;
    struct list_node *list_tmp = make_list;
    fprintf(fp, ".DEFAULT_GOAL := all\n.PHONY: all\nCFLAGS= -W -Wall -Wextra\n\nall: "); 
    char name_cut[NAME_MAX];
    while(list_tmp != NULL){
        make_tmp = list_get_last_node(list_tmp->node);
        sprintf(name_cut, "%s",make_tmp->node->name);
        char *p;
        p = strchr( name_cut, '.');
        if (p != NULL) {
            *p = 0;
        }
        fprintf(fp,"%s ",name_cut);
        list_tmp = list_tmp->next->next;
    }
    fprintf(fp,"\n\n");
}
void print_make_body(FILE *fp,struct list_node *make_list){
    struct make_node *make_tmp = NULL;
    struct list_node *list_tmp = make_list;
    list_tmp = make_list;
    int corh = 1;
    char name_upper[NAME_MAX];
    char name_cut[NAME_MAX];
    while(list_tmp != NULL){
        if(corh %2){
            make_tmp = list_get_last_node(list_tmp->node);
            sprintf(name_upper, "%s",make_tmp->node->name);
            sprintf(name_cut, "%s",make_tmp->node->name);
            char *p;
            p = strchr(name_upper, '.');
            if (p != NULL) {
                *p = 0;
            }
            to_upper(name_upper);
            fprintf(fp,"%sOBJ= ",name_upper);
        }/*else{
            fprintf(fp,"%sDEP= ",name_upper);
        }*/
        make_tmp = list_tmp->node;     
        while(make_tmp != NULL){
            char name[NAME_MAX];
            strcpy(name,make_tmp->node->name);
            if(dotO == 1 && corh % 2){

               char *p;
                p = strchr(name, '.');
                if (p != NULL) {
                    *(p+1) = 'o';
                } 
            }
            fprintf(fp,"%s/%s ",make_tmp->node->path, name);
            make_tmp = make_tmp->next;
        }
        fprintf(fp,"\n\n");
        corh++;
        if(corh % 2){
            fprintf(fp,"%s: $(%s) ($%s)\n\n",name_cut,name_upper,name_upper);
        }
        list_tmp = list_tmp->next;
    }
}
void print_make_footer(FILE *fp,struct list_node *make_list){
    struct make_node *make_tmp = NULL;
    struct list_node *list_tmp = make_list;
    char name_cut[NAME_MAX];
    
    fprintf(fp,"clean:\n\t$(RM) ");
    while(list_tmp != NULL){
        make_tmp = list_get_last_node(list_tmp->node);
        strcpy(name_cut,make_tmp->node->name);
        
        char *p;
        p = strchr( name_cut, '.');
        if (p != NULL) {
            *p = 0;
        }
        fprintf(fp,"%s ",name_cut);
        list_tmp = list_tmp->next->next;
    }
    fprintf(fp,"*.o");
}
void print_make(FILE *fp,struct list_node *make_list){
    
    print_make_header(fp,make_list);
    print_make_body(fp,make_list);
    print_make_footer(fp,make_list);
}