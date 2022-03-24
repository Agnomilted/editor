#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define COMMANDMAXSIZE 4096
#define COMMANDARGNUMLIMIT 5

char filename[FILENAME_MAX];
char* projectdir = "/tmp/editor";
char *commandprompt = ": ";
char *inputprompt = "> ";
int32_t commandpromptlen;
char *defaulteditor = "vim";

struct flags{
	int a0 : 1; //specified file name in arguments
	int a1 : 1; //


	char editorprogram[FILENAME_MAX];
} proj;

void print(char* s){
	write(1, s, strlen(s));
}

void prompt(char* s){
	int32_t len = commandpromptlen + strlen(s);
	char *out = malloc(len);
	concatenate(out, commandprompt, s);
	write(1, out, len);
	free(out);
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

void fetchenvvars(void){
	char *a = getenv("EDITOR");
	if(!a)
		strcpy(proj.editorprogram, defaulteditor);
	else 
		strcpy(proj.editorprogram, a);

}

int main (int argc, char** argv){
	proj.a0 = 0;
	commandpromptlen = strlen(commandprompt);

	fetcharguments(argc, argv);
	fetchenvvars();
	struct stat st = {0};
	if(!stat(projectdir, &st)){
		prompt("there are files in the temporary project directory, do you want to overwrite them ?(y/n)\n");
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
	char commandargs[COMMANDARGNUMLIMIT][COMMANDMAXSIZE];
	char command[COMMANDMAXSIZE], output[COMMANDMAXSIZE];
	int32_t i, j;
	int8_t f1;
	int32_t len, len2;
	for(;;){
		i = 0, j = 0, len2 = 0, f1 = 0;
		print(inputprompt);
		len = read(0, command, COMMANDMAXSIZE);
		for(;i<len && j < COMMANDARGNUMLIMIT;++i){
			if(command[i] == '\"'){
				commandargs[j][len2] = '\0';
				f1 = !f1;
			}
			else if(command[i] == ' ' && !f1){
				commandargs[j][len2] = '\0';
				++j;
				len2 = 0;
			}
			else {
				commandargs[j][len2] = command[i];
				++len2;
			}
		}
		commandargs[j][len2 - 1] = '\0';
		
		if(!strcmp(commandargs[0], "edit")){
			concatenate(output, projectdir, "/");
			concatenate(output, output, commandargs[1]);
			char *a[3] = { proj.editorprogram, output, 0 };
			spawn(a);
		}
		else if(!strcmp(commandargs[0], "write")){
			prompt("write mode\n");
			concatenate(output, "written file is ", commandargs[1]);
			prompt(output);

		}
		else if(!strcmp(commandargs[0], "yarak")){
			prompt("yes yarak!");
		}
		else {
			prompt("unknown command!");
		}
		putchar('\n');
		memset(commandargs, '\0', COMMANDMAXSIZE * COMMANDARGNUMLIMIT);
		memset(command, '\0', COMMANDMAXSIZE);
		memset(output, '\0', COMMANDMAXSIZE);

	}

	return 0;
}
