#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

char filename[FILENAME_MAX];
char* projectdir = "/tmp/editor";
struct flags{
    int a0 : 1; //specified file name in arguments
} proj;

void print(char* s){
    write(1, s, strlen(s));
}

void spawn(char** args){
    if(!fork()){
        execvp(args[0], args);
        exit(0);
    }
    wait(0);
}

int32_t concatenate(char* dest, char* a, char* b){
    int32_t ptr1 = 0, ptr2 = 0;
    while (a[ptr1]){
        dest[ptr1] = a[ptr1];
        ++ptr1;
    }
    while(b[ptr2]){
        dest[ptr1 + ptr2] = b[ptr2];
        ++ptr2;
    }
    return ptr1 + ptr2 - 1;
}

void fetcharguments(int argc, char** argv){
    for (int32_t i = 1; i < argc; ++i){
        if(!strcmp(argv[i], "-yarak")){
        }
        else {
            strcpy(filename, argv[i]);
            proj.a0 = 1;
        }
    }
}

int main (int argc, char** argv){
    proj.a0 = 0;

    fetcharguments(argc, argv);

    struct stat st = {0};
    if(!stat(projectdir, &st)){
        print("there are files in the temporary project directory, do you want to overwrite them ?(y/n)\n");
        if(getchar() == 'y'){
            char amog[65536];
            if(proj.a0){
                char *a[4] = {"rm", "-rf", projectdir, 0}, *b[5] = {"cp", "-r", filename, projectdir, 0};
                spawn(a);
                spawn(b);
            }
            else{
                char *a[4] = {"rm", "-rf", projectdir, 0};
                spawn(a);
                mkdir(projectdir, 0770);
            }
        }
    }
    else if (stat(projectdir, &st) == -1){
        mkdir(projectdir, 0770);
    }

    return 0;
}
