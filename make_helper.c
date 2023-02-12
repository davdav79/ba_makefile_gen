#include "make_helper.h"
#include <stdbool.h>

FILE *make_init(){
    static char * name = "Makefile_";
    const char * cmd = {"mv Makefile_ Makefile_.bak"};
    printf("moving Makefile to Makefile.bak\n");
    int status = system(cmd);
    if (status != 0) {
        fprintf(stderr, "Error: Unable move Makefile to MAkefile.bak\n");
    }
    FILE* fp = fopen(name, "w+");
    if (fp == NULL) {
        perror(name);
        return NULL;
    }
    return fp;
}

void make_list_node(struct list_node *make_list, struct node *node){
    char libs[2][16] = {"pthread.h", "math.h"};
    char libs_name[2][16] = {"-lpthread", "-lm"};
    if(node->is_duplicate)
        return;
    if( false == node->is_local)
        return;
    if(node->name[strlen(node->name)-1] == 'c'){ //if the file is a .c it gets appended to obj
        list_insert_node((struct list_node **)&make_list->data,node);
    }
    struct list_node *node_list = node->leaves;
    while(node_list != NULL){
        for(unsigned int i=0;i<sizeof(libs)/sizeof(libs[0]);i++){
            if(strcmp(libs[i],((struct node*)node_list->data)->name) == 0){
                struct node* found  = find_file(libs_name[i],(struct list_node *)make_list->next->data);
                if(found == NULL){
                    list_insert_node((struct list_node **)&make_list->next->data,new_node(libs_name[i],"",0,0));
                }
            }    
        }
        node_list = node_list->next;
    }
}

void print_make_header(FILE *fp,struct list_node *make_list){
    struct list_node *make_tmp = NULL;
    struct list_node *list_tmp = make_list;
    fprintf(fp, ".DEFAULT_GOAL := all\n.PHONY: all\nCFLAGS= -W -Wall -Wextra\n\nall: "); 
    char name_cut[NAME_MAX];
    while(list_tmp != NULL){
        make_tmp = list_get_last_node((struct list_node*)list_tmp->data);
        sprintf(name_cut, "%s",((struct node*)make_tmp->data)->name);
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
    struct list_node *make_tmp = NULL;
    struct list_node *list_tmp = make_list;
    list_tmp = make_list;
    int corh = 1;
    char name_upper[NAME_MAX];
    char name_cut[NAME_MAX];
    while(list_tmp != NULL){
        if(corh %2){
            make_tmp = list_get_last_node(list_tmp->data);
            sprintf(name_cut, "%s",((struct node*)make_tmp->data)->name);
            char *p;
            p = strchr(name_cut, '.');
            if (p != NULL) {
                *p = 0;
            }
            strcpy(name_upper, name_cut);
            to_upper(name_upper);
            fprintf(fp,"####################%s####################\n\n",name_upper);
            fprintf(fp,"%sOBJ= ",name_upper);
        }else{
            fprintf(fp,"%sLIB= ",name_upper);
        }
        make_tmp = list_tmp->data;     
        while(make_tmp != NULL){
            char name[NAME_MAX];
            strcpy(name,((struct node*)make_tmp->data)->name);
            if(arguments.object == 1 && corh % 2){

               char *p;
                p = strchr(name, '.');
                if (p != NULL) {
                    *(p+1) = 'o';
                } 
            }
            if(strlen(((struct node*)make_tmp->data)->path)>0)
                fprintf(fp,"%s/%s ",((struct node*)make_tmp->data)->path, name);
            else
                fprintf(fp,"%s ", name);
            make_tmp = make_tmp->next;
            if(corh %2){
                if(make_tmp == NULL){
                    break;
                }
                if(make_tmp->next == NULL){
                    break;
                }
            }
        }
        fprintf(fp,"\n\n");
        corh++;
        if(corh % 2){
            fprintf(fp,"%s: $(%sOBJ) ($%sLIB)\n\n",name_cut,name_upper,name_upper);
        }
        list_tmp = list_tmp->next;
    }
}
void print_make_footer(FILE *fp,struct list_node *make_list){
    struct list_node *make_tmp = NULL;
    struct list_node *list_tmp = make_list;
    char name_cut[NAME_MAX];
    
    fprintf(fp,"clean:\n\t$(RM) ");
    while(list_tmp != NULL){
        make_tmp = list_get_last_node(list_tmp->data);
        strcpy(name_cut,((struct node*)make_tmp->data)->name);
        
        char *p;
        p = strchr( name_cut, '.');
        if (p != NULL) {
            *p = 0;
        }
        fprintf(fp,"%s %s.png ",name_cut,name_cut);
        list_tmp = list_tmp->next->next;
    }
    fprintf(fp,"*.dot *.o");
}
void print_make(FILE *fp,struct list_node *make_list){
    printf("Generating Makefile\n");
    printf("Writing Makeheader\n");
    print_make_header(fp,make_list);
    printf("Writing Makebody\n");
    print_make_body(fp,make_list);
    print_make_footer(fp,make_list);
    printf("Writing Makeheader\n");
}