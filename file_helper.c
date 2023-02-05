#include "file_helper.h"
#include <limits.h>

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
char** parse_for_include(char *file_name, char *path){ 
    char file_path[PATH_MAX+NAME_MAX];
    sprintf(file_path, "%s/%s",path,file_name);
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Error:");
        return NULL;
    }
    int file_count = 0, max_files = 1;
    char **files = malloc(max_files * sizeof(char *));
    
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
        files[file_count++] = malloc(NAME_MAX*sizeof(char));
        strcpy(files[file_count-1], include_file_name);
        //printf("include files %s/%s: %s\n",path,file_name,files[file_count-1]);
    }
    // add an empty node to indicate the end of the list
    if(file_count ==  max_files){
                max_files = max_files * 2;
                files = realloc(files,max_files * sizeof(struct node*));
    } 
    files[file_count] = malloc(NAME_MAX*sizeof(char));
    files[file_count] = 0;
    return files;
}

struct node * find_file(char* path, char* file_name){
    //printf("find file: %s_%s\n",path,file_name);
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
    int i = 0;
    struct node * include_file = NULL;
    char * delim = NULL;
    while(files[i] != 0){
        delim = strrchr(files[i],'/');
        if(delim){
            files[i][delim-files[i]] = '\0';
            if('.' == files[i][0]){
                include_file = find_file(files[i], delim+1);
            }else{
                char path[PATH_MAX+NAME_MAX];
                sprintf(path, "%s/%s",root->path,files[i]); 
                include_file = find_file(path, delim+1);
            }
            
        }else{
            include_file = find_file(root->path,files[i]);
        }
        if(NULL != include_file){
            if('c' == (root->name[strlen(root->name)-1]) && 0 == is_main){
                insert_existing_node(include_file,root);
            }else{
                insert_existing_node(root,include_file);
            }
        }
        else{
            insert_new_node(root,files[i],".",5,0);
        }
        free(files[i]);
        i++;
    }
    free(files[i]);
    free(files);   
}


/// @brief parses the given directory, calls itself if it finds a directory or calls file_to_node if it finds a file
/// @param path //path to the directory
/// @return -1 when it cant open the directory 0 on success

int parse_directory(char* path){
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if (!d) {
        printf("could not open the directory: \n");
        return -1;
    }
    while ((dir = readdir(d)) != NULL) {
        if(dir->d_type == DT_DIR){   
            if(dir->d_name[0] == '.')//to not get an infinite loop where we look into the current directory
                continue;
            char * new_path = malloc((strlen(path)+strlen(dir->d_name))*sizeof(char));
            sprintf(new_path,"%s/%s",path,dir->d_name);
            if(parse_directory(new_path)< 0){
                free(new_path);
                return -1;
            }
            free(new_path);
        }else if(DT_REG){
            file_to_node(dir,path);
        }
        
    }
    closedir(d);
    return 0;
}

void reset_duplicate(){
    for(int i = 0; i<num_source_files;i++){
        source_files[i]->is_duplicate = 0;
    }
}