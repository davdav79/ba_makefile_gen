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
#include "argp_helper.h"

const char *argp_program_version = "Version 1.0";
const char *argp_program_bug_address = "https://github.com/davdav79/ba_makefile_gen/issues";

extern int dotO;
extern struct arguments arguments;

int main(int argc, char *argv[]){   
    /* Our argp parser. */
    struct argp argp = {options, parse_opt, args_doc, doc, NULL, NULL, NULL};
    
    /* Default values. */
    arguments.object = 1;
    arguments.output = "";
    arguments.path = ".";
    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    if(argc >1){
        argp_parse(&argp, argc, argv, 0, 0, &arguments);
    }
    
    dotO = 0;
    struct list_node *source_files = NULL;
    struct list_node *main_files = NULL;
    if(0 > parse_directory(arguments.path,&source_files,&main_files)){
        return -1;
    }
    struct list_node *tmp_source_files = source_files;
    while(tmp_source_files != NULL){
        parse_for_insert_and_create_node(((struct node*)tmp_source_files->data),&source_files,0);
        tmp_source_files = tmp_source_files->next;
    }
    tmp_source_files = source_files;

    FILE * dot_file, *make_file;
    struct list_node *make_list = NULL;
    make_file = make_init();
    struct list_node *tmp_main_files = main_files;
    while(NULL != tmp_main_files){
        list_insert_node(&make_list,NULL);
        list_insert_node(&make_list,NULL);
        printf("main file: %s\n", ((struct node*)tmp_main_files->data)->name); 
        parse_for_insert_and_create_node(((struct node*)tmp_main_files->data),&source_files,1);
        dot_file = print_dot_init(((struct node*)tmp_main_files->data)->name);
        if(dot_file == NULL){
            return -1;
        }
        in_order_traversal(dot_file,make_list,((struct node*)tmp_main_files->data));
        fprintf(dot_file,"}\n");
        fclose(dot_file);
        exec_dot(((struct node*)tmp_main_files->data)->name);
        reset_duplicate(source_files);
        tmp_main_files = tmp_main_files->next;
    }
        print_make(make_file,make_list);
    return 0;
}