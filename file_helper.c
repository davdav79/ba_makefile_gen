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

void parse_for_insert_and_create_node(struct node** root, struct list_node **source_files){ 
    // Open the file
    char file_path[PATH_MAX+NAME_MAX];
    char *file_name = (*root)->name;
    char *path = (*root)->path;
    struct list_node *file_list = NULL;
    sprintf(file_path, "%s/%s",path,file_name);
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Error: Could not open file");
        return;
    }
    // Read the file line by line
    char line_array[2048];
    char search[2][10] = {"voidmain(", "intmain("};
    int search_cnt[2] = {0,0};
    char search_include[10] = {"#include"};
    int search_include_cnt = 0;
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
                    last_char = '\n';
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

            //searching for main function
            for(int i = 0; i<2; i++){
                if(search[i][search_cnt[i]] == *line){
                    search_cnt[i]++;
                    if(search_cnt[i] == (int)strlen(search[i])){
                        (*root)->is_main = 1;
                    }
                }else{
                    search_cnt[i] = 0;
                }
            }
            //searching for include
            if(search_include[search_include_cnt] == *line){
                search_include_cnt++;
                if(search_include_cnt == (int)strlen(search_include)){
                    while((*line != '\"') && (*line != '<')){
                        last_char = *line;
                        line++;
                    }
                    last_char = *line;
                    line++;
                    char *include_start = line, cmp_char ='\"';
                    if('<' == last_char){
                        cmp_char = '>';
                    } 
                    int len = 0;
                    while(cmp_char != *line){
                        len++;
                        last_char = *line;
                        line++;
                    }
                    char *found_include = malloc((len+2)*sizeof(char));
                    strncpy(found_include,include_start,len);
                    *(found_include+len) = '\0';
                    char *path_name = NULL;
                    if(('\"' == cmp_char) && ('.' != *found_include)){
                        path_name = malloc((strlen(path)+ strlen(found_include)+2)*sizeof(char));
                        sprintf(path_name,"%s/%s",path,found_include); 
                    }else{
                        path_name = malloc((strlen(found_include)+2)*sizeof(char));
                        sprintf(path_name,"%s",found_include);
                        
                    }
                    free(found_include);
                    list_insert_node(&file_list,path_name);
                    search_include_cnt = 0;
                }
            }else{
                search_include_cnt = 0;
            }

            last_char = *line;
            line++;
        }
    }
    while(file_list != NULL){
        insert_and_append_node(source_files,*root,(char*)file_list->data);
        free(file_list->data);
        list_remove_node(&file_list);
    }
    
    return;
}


void insert_and_append_node(struct list_node **list,struct node* root, char* path_name){
    struct node *include_file;
    char * delim = strrchr(path_name,'/');
    if(delim){
            path_name[delim-path_name] = '\0';
            include_file = find_file(delim+1, *list);
    }else{
        include_file = find_file(path_name,*list);
    }
    if(NULL != include_file){
        if(0 == root->is_main && ('c' == (root->name[strlen(root->name)-1]))){
            append_existing_node(include_file,root);
        }else{
            append_existing_node(root,include_file);
        }
    }
    else{
        if(delim){
            list_insert_node(list,new_node(delim+1, path_name, 0,0));
            append_existing_node(root,((struct node*)((struct list_node*)*list)->data));
        }else
        {
            list_insert_node(list,new_node(path_name, "", 0,0));
            append_existing_node(root,((struct node*)((struct list_node*)*list)->data));
        }
    }
}

struct node *find_file(char* name, struct list_node *list){
    while(NULL != list){
        //printf("find file:%s,%s\n",((struct node*)list->data)->name,name);
        if(0 == strcmp( ((struct node*)list->data)->name,name)){
            return ((struct node*)list->data);
        }
        list = list->next;
    }
    return NULL;
}
int file_to_node(struct dirent *dir, char *path, struct list_node **source_files){
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
    list_insert_node(source_files,new_node(dir->d_name, path, 1,0));
    return 0;
}

/// @brief parses the given directory, calls itself if it finds a directory or calls file_to_node if it finds a file
/// @param path //path to the directory
/// @return -1 when it cant open the directory 0 on success

int parse_directory(char* path, struct list_node ** source_files){
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
            if(parse_directory(new_path, source_files) == -1){
                free(new_path);
                return -1;
            }
            free(new_path);
        }else if(DT_REG){
            file_to_node(dir, path, source_files);
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
