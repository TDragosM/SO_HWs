#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int str_ends_with(const char *str, const char *suffix) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (str_len < suffix_len) {
        return 0;
    }
    return !strcmp(str + str_len - suffix_len, suffix);
}

int listDir(const char *path, int has_perm_execute, const char *name_ends_with)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[512];
    struct stat statbuf;

    dir = opendir(path);
    if(dir == NULL) {
        perror("Could not open directory");
        return -1;
    }
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(filePath, 512, "%s/%s", path, entry->d_name);
            if(lstat(filePath, &statbuf) == 0) {
                if (!name_ends_with || str_ends_with(entry->d_name, name_ends_with)) {
                    if (has_perm_execute && ((statbuf.st_mode & S_IXUSR) == 0)) {
                        continue; // nu are permisiune de executie, trecem la urmatorul
                    } else {
                        printf("%s\n", filePath);
                    }
                }
                
            } else {
                printf("[err] ");
            }
        }
    }

    closedir(dir);
    return 0;
}


void listRec(const char *path, int has_perm_execute, const char *name_ends_with) {
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;

    dir = opendir(path);
    if (dir == NULL) {
        perror("Could not open directory");
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if (lstat(fullPath, &statbuf) == 0) {
                if (has_perm_execute && ((statbuf.st_mode & S_IXUSR) == 0)) {
                    continue; // nu are permisiune de executie, trecem la urmatorul
                } else if (name_ends_with != NULL && !S_ISDIR(statbuf.st_mode)) {
                    // verificăm dacă numele elementului se termină cu secvența de caractere
                    if (strstr(entry->d_name, name_ends_with) == NULL) {
                        continue;
                    }
                }
                printf("%s\n", fullPath);
                
                if (S_ISDIR(statbuf.st_mode)) {
                    listRec(fullPath, has_perm_execute, name_ends_with);
                }
            }
        }
    }

    closedir(dir);
}
void ParseFile(const char* path){
    int f=open(path, O_RDONLY);
    if(f == -1){
        printf("ERROR\ncould not open file\n");
        return;
    }
    lseek(f, -1, SEEK_END);
    unsigned char magic;
    read(f,&magic,1);
    
    if(magic!='1'){
        printf("ERROR\nwrong magic\n");
        close(f);
        return;
    }
    lseek(f,-3,SEEK_END);
    short int header_size;
    read(f, &header_size,sizeof(header_size));
    lseek(f, -header_size, SEEK_END);
    int version;
    read(f, &version,sizeof(version));
    if(version<105 || version>176){
        printf("ERROR\nwrong version\n");
        close(f);
        return;
    }
    short int nr_of_sections;
    
    read(f, &nr_of_sections,1);
    if(nr_of_sections<6 || nr_of_sections>17){
        printf("ERROR\nwrong sect_nr\n");
        close(f);
        return;
    }
    short int section_type;
        for(int i=0; i<nr_of_sections; i++){
            lseek(f,13,SEEK_CUR);
            read(f, &section_type,sizeof(section_type));
            if(section_type!=82 && section_type!=38){
                printf("ERROR\nwrong sect_types\n");
                close(f);
                return;
            }
            lseek(f, 8, SEEK_CUR);
        }
    
    lseek(f,-header_size+5, SEEK_END);
    printf("SUCCESS\n");
    printf("version=%d\n",version);
    printf("nr_sections=%d\n",nr_of_sections);
    for(int i=0;i<nr_of_sections;i++){
        char section_name[14];
        int section_offst;
        int section_size;
        read(f, section_name,13);
        section_name[13]='\0';
        read(f, &section_type,sizeof(section_type));
        read(f, &section_offst,sizeof(section_offst));
        read(f, &section_size,sizeof(section_size));
        printf("section%d: %s %hd %d\n",i+1,section_name,section_type,section_size);
    }
    close(f);
}


int main(int argc, char **argv) {
   
    char* path = NULL;
    int recursive = 0;
    char* name_ends_with = NULL;
    int has_perm_execute = 0;
    int cmdList=0;
    int cmdParse = 0;

   
    for(int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "variant") == 0) {
        printf("67674\n");
        return 0;
    }
    if(strcmp(argv[i], "list") == 0) {
        cmdList = 1;
    }
    if(strcmp(argv[i], "parse")==0){
        cmdParse = 1;
    }

    if(strncmp(argv[i], "path=", 5) == 0) {
        path = argv[i] + 5;
    }
    else if(strcmp(argv[i], "recursive") == 0) {
        recursive = 1;
    }
    else if(strncmp(argv[i], "name_ends_with=", 15) == 0) {
        name_ends_with = argv[i] + 15;
    }
    else if(strcmp(argv[i], "has_perm_execute") == 0) {
        has_perm_execute = 1;
    }

    
}
    if(cmdList == 1) {
        
    if(path == NULL ) {
        perror("ERROR\ninvalid directory path\n");
        return -1;
        
        exit(EXIT_FAILURE);
    }
    
    if(recursive == 0) {
        printf("SUCCESS\n");
        
        listDir(path,has_perm_execute,name_ends_with);
    }

    if(recursive==1){
        printf("SUCCESS\n");
        listRec(path,has_perm_execute,name_ends_with);
    }
    }
    if(cmdParse == 1) {
        //printf("SUCCESS\n");
        ParseFile(path);
    }
    
    return 0;
}
