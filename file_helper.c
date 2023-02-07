#include "file_helper.h"

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

//finds all #include <...> annd #include"..."  and if it doesent exist, adds it to the file list.
void parse_for_insert_and_create_node(struct node* root, struct list_node **source_files,int is_main){ 
    char file_path[512];
    char *file_name = root->name;
    char *path = root->path;
    sprintf(file_path, "%s/%s",path,file_name);
    FILE *file = fopen(file_path, "r");
    //printf("parent: path:%s name:%s\n",path,file_name);
    if (file == NULL) {
        perror("Error: Could not open the file");
        return;
    }
    // Read the file line by line
    char line[1024];
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
        char tmp[2] = "";
        if(include_file != NULL){
            include_file_end = strstr(line, ">");
            path = "";
        }
        else{
            include_file = strstr(line, "\"");
            include_file_end = strstr(include_file+1, "\"");
            sprintf(tmp,"/");
        }
        if(include_file == NULL)
            continue;
        int include_file_len = include_file_end - include_file-1;
        //char include_file_name[include_file_len];
        char path_name[PATH_MAX+NAME_MAX];
        include_file[include_file_len+1] = '\0';
        sprintf(path_name,"%s%s%s",path,tmp,include_file+1);
        insert_and_append_node(source_files,root,path_name,is_main);
    }
}


void insert_and_append_node(struct list_node **list,struct node* root, char* path_name, int is_main){
    
    struct node *include_file;
    char * delim = strrchr(path_name,'/');
    if(delim){
            path_name[delim-path_name] = '\0';
            include_file = find_file(delim+1, *list);
    }else{
        include_file = find_file(path_name,*list);
    }
    if(NULL != include_file){
        if('c' == (root->name[strlen(root->name)-1]) && 0 == is_main){
            append_existing_node(include_file,root);
        }else{
            append_existing_node(root,include_file);
        }
    }
    else{
        if(delim){
            list_insert_node(list,new_node(delim+1, path_name, 0));
            append_existing_node(root,((struct node*)((struct list_node*)*list)->data));
        }else
        {
            list_insert_node(list,new_node(path_name, "", 0));   
            append_existing_node(root,((struct node*)((struct list_node*)*list)->data));
        }
    }
}

struct node *find_file(char* name, struct list_node *list){
    //printf("\tfind file: %s\n",path);
    //printf("find_file path: %s name: %s\n", path, name);
    while(NULL != list){
        if(0 == strcmp( ((struct node*)list->data)->name,name)){
            //printf("found file: %s/%s\n",((struct node*)list->data)->path,((struct node*)list->data)->name);
            return ((struct node*)list->data);
        }
        list = list->next;
    }
    return NULL;
}
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

/// @brief parses the given directory, calls itself if it finds a directory or calls file_to_node if it finds a file
/// @param path //path to the directory
/// @return -1 when it cant open the directory 0 on success

int parse_directory(char* path, struct list_node ** source_files, struct list_node ** main_files){
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if (!d) {
        perror("Error: could not open the directory:");
        return -1;
    }
    while ((dir = readdir(d)) != NULL) {
        if(dir->d_type == DT_DIR){   
            if(dir->d_name[0] == '.')//to not get an infinite loop where we look into the current directory
                continue;
            char * new_path = malloc((strlen(path)+strlen(dir->d_name)+1)*sizeof(char));
            //printf("malloc%ld\n",new_path);
            sprintf(new_path,"%s/%s",path,dir->d_name);
            if(parse_directory(new_path, source_files, main_files) == -1){
                free(new_path);
                return -1;
            }
            free(new_path);
        }else if(DT_REG){
            file_to_node(dir, path, source_files, main_files);
        }
        
    }
    closedir(d);
    if(*source_files == NULL){
        fprintf(stderr,"Error: no project files in folder\n");
        return -2;
    }
    return 0;
}
void reset_duplicate(struct list_node *list){
    while(list){
        ((struct node *)list->data)->is_duplicate = 0;
        //printf("\treset dupe: %s is dupe: %d\n",((struct node *)list->data)->name,((struct node *)list->data)->is_duplicate);
        list = list->next;
    }
}


//checks if the file contains a main function 
int check_if_main(char * file_name, char *path){
    // Open the file
    char file_path[PATH_MAX+NAME_MAX];
    sprintf(file_path, "%s/%s",path,file_name);
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Error: Could not open file");
        return 2;
    }
        // Read the file line by line
    char line_array[2048];
    char search[2][10] = {"voidmain(", "intmain("};
    int search_cnt[2] = {0,0};
    char last_char = '\0';
    char * line = NULL;
    int comment = 0, block_comment = 0,string_double = 0,string_single = 0;

    while (fgets(line_array, sizeof(line_array), file)) {
        line = line_array;
        while('\0' != *line){
            //ignore whitespaces
            if(isspace(*line)){
                line++;
                continue;
            }
            //ignore for comments
            if(0 == string_double && 0 == string_single ){
                comment = check_if_comment(last_char,*line,&block_comment);
                if(2 == comment){
                    break;
                }else if(1 == comment){
                    last_char = *line;
                    line++;
                    continue;                
                }
            }
            //ignore string literals
            if(check_if_string_literal(last_char,*line,&string_double,&string_single)){
                last_char = *line;
                line++;
                continue;
            }
            for(int i = 0; i<2; i++){
                if(search[i][search_cnt[i]] == *line){
                    search_cnt[i]++;
                    if(search_cnt[i] == (int)strlen(search[i])){
                        return 0;
                    }
                }else{
                    search_cnt[i] = 0;
                }
            }
            last_char = *line;
            line++;
        }
    }
    return 1;
}
