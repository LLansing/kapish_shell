#define _GNU_SOURCE

/*------------------------------------------------------------------------------
    Lance Lansing 
    CSC360 Assignment 1 - kapish shell
    January 2019
------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
//auxillary file containing extra functions
#include "utilfunctions.h"

#define MAX_INPUT_LENGTH 512	//max length of command input
#define TOKEN_BUFFSIZE 64	//max number of tokens taken from input
#define TOKEN_DELIM " \t\r\n\a" //delimiting characters for tokenizing
#define LINE_BUFFSIZE 100 //max number of lines taken from rc file
#define MAX_RCPATH_LENGTH 100 //max number of chars taken for rcpath

#define UNUSED(expr) do { (void)(expr); } while (0) //for silencing unused error

//text colors
#define KGRN  "\x1B[32m"
#define KNRM  "\x1B[0m"
//FUNCTION DECLARATIONS---------------------------------------------------------

int kapish_cd(char **args);
int kapish_exit(char **args);
int kapish_setenv(char **args);
int kapish_unsetenv(char **args);

void kapish_loop(void);
char *kapish_read_line(void);
char **kapish_tokenize(char *line);
int kapish_execute(char **args);
int kapish_launch(char **args);

void kapishrc_init(void);

void INThandler(int sig);

//GLOBALS-----------------------------------------------------------------------
char *ignore_cmd = "kapishignore";	//ignore command for skipping execution

//list of builtin commands, followed by builtin functions
char *builtin_str[] = {
	"cd",
	"exit",
	"setenv",
	"unsetenv"
};

int (*builtin_func[]) (char **) = {
	&kapish_cd,
	&kapish_exit,
	&kapish_setenv,
	&kapish_unsetenv
};

int kapish_num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}

/*BUILTIN COMMANDS--------------------------------------------------------------
	kapish_cd - change directory, go to home if no directory specified
	kapish_exit - return 0, ending the command loop
	kapish_setenv - sets a given environment variable to a given value, creating
									it if it does not exist
	kapish_unsetenv - deletes given environment variable */

/*changes directory utilizing chdir() and goes to home directory if no location
	was specified*/
int kapish_cd(char **args){
	char *destdir = args[1];
	if(destdir == NULL) {
		//no location specified - go to home directory
		destdir = getenv("HOME");
		if(destdir == NULL){
			printf("HOME environment variable not found\n");
			return 1;
		}
	}
	//change directory and give error if something went wrong
	if(chdir(destdir) != 0){
		perror("kapish");
	}
	return 1;	//1 signals a finished command and allows command loop to continue
}

/*returns 0 - signal for command loop to terminate*/
int kapish_exit(char **args){
	return 0;
}

/*sets and environment variable using setenv(). args[0] is the command name,
	args[1] is the variable to set/create, and args[2] is the value to set it to*/
int kapish_setenv(char **args){
	char *varname = args[1];
	char *value = args[2];
	if(value == NULL){
		value = "";
	}

	//int of 1 allows overwriting of existing variables
	setenv(varname, value, 1);
	return 1;
}

/*remove environment variable, first checking if it exists*/
int kapish_unsetenv(char **args){
	if(args[1] == NULL){
		printf("No environment variable received\n");
		return 1;
	}
	if(getenv(args[1]) == NULL){
		printf("No such environment variable exists\n");
	}
	//unset variable
	if(unsetenv(args[1]) == -1){
		perror("kapish");
	}
	return 1;
}

/*ENGINE COMMANDS---------------------------------------------------------------
	kapish_loop - performs loopo of the shell
	kapish_read_line - stores line from input
	kapish_tokenize - tokenizes line into array of arguments
	kapish_execute - appropriately runs a builtin or launches the given command
	kapish_launch - fork() and execvp() to execute given command*/

/*Engine of the shell - loops through input and executes accordingly via calls
  to other functions. Uses status variable returned by execution command to
  determine when to exit the loop.*/
void kapish_loop(void){
  char *line;
  char **args;
  int status=0;

  do{
		//print prompt with date and time (get time function in utilfunctions.c)
    printf("%skapish - %s%s ? ", KGRN, get_time_str(), KNRM);
    line = kapish_read_line();
		//if input length was exceeded or command was "kapishignore", continue
		if(!strncmp(line, ignore_cmd, strlen(ignore_cmd))){
			status = 1;
			continue;
		}
    args = kapish_tokenize(line);
    status = kapish_execute(args);
		if(line != NULL){ free(line); }
		if(args != NULL){ free(args); }

  } while(status);
}

/*reads line from stdin char by char using getchar()*/
char *kapish_read_line(void){
  char *buffer = malloc(sizeof(char) * MAX_INPUT_LENGTH);
  int position = 0;
  int ch;

  //error-handler for allocation failure
  if(!buffer){
    fprintf(stderr, "kapish: allocation of buffer error\n");
		free(buffer);
    exit(EXIT_FAILURE);
  }

  while(1){
    //read initial character
    ch = getchar();
		signal(SIGINT, INThandler);//for handling ctrl-c

		//handles ctrl-d input by interpreting it as "exit"
		if(ch == EOF && position == 0){
			strncpy(buffer, "exit", 4);
			buffer[4] = '\0';
			return buffer;
		}
    //once EOF or newline is read, replace with null character and return
    //other wise add ch to buffer and make sure length has not been exceeded
    if(ch == EOF || ch == '\n'){
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = ch;
    }
    position++;

    //input has exceeded max length and buffer will be changed to the ignore_cmd
    if(position >= MAX_INPUT_LENGTH){
      printf("kapish: input has exceeded maximum length and will be ignored\n");
      buffer[position] = '\0';
			free(buffer);
      return ignore_cmd;
    }
  }
}


/*separates line into array of tokens, increasing token array size if needed*/
char **kapish_tokenize(char *line){
	int buffsize = TOKEN_BUFFSIZE, position = 0;
	char **tokens = malloc(sizeof(char*) * buffsize);
	char *token;

	//error-handler for failed allocation
	if(!tokens){
		fprintf(stderr, "kapish: allocation of token array error\n");
		free(line);
		free(tokens);
		exit(EXIT_FAILURE);
	}

	//tokenizing loop with strtok - reallocate if needed, error-handle realloc
	token = strtok(line, TOKEN_DELIM);
	while(token != NULL){
		tokens[position] = token;
		position++;

		if(position >= buffsize){
			buffsize += TOKEN_BUFFSIZE;
			tokens = realloc(tokens, sizeof(char*) * buffsize);
			if(!tokens){
				fprintf(stderr, "kapish: allocation of token array error\n");
				free(line);
				free(tokens);
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, TOKEN_DELIM);
	}
	//make trailing position in token array NULL
	tokens[position] = NULL;
	return tokens;
}

int kapish_execute(char **args){
	int i;

	//if empty command was entered, return 1 (do nothing)
	if(args[0] == NULL){
		return 1;
	}

	//loop through the builtins, executing if input matches
	for(i = 0; i < kapish_num_builtins(); i++){
		if(strncmp(args[0], builtin_str[i], strlen(builtin_str[i])) == 0){
			return (*builtin_func[i])(args);
		}
	}

	return kapish_launch(args);
}

/*forks a child process where the arguments are given to execvp() to execute the
	given command. Parent process waits for termination of child then returns 1,
	 telling the calling function that a input prompt should be performed again*/
int kapish_launch(char **args){
	pid_t pid, wpid;
	int status;

	pid = fork();
	if(pid == 0){
		//child process - fork has returned 0 to pid
		//if execvp returns, something has go#define KNRM  "\x1B[0m"ne wrong, otherwise the given command
		//will execute
		if (execvp(args[0], args) == -1){
			perror("kapish");
		}
		exit(EXIT_FAILURE);//this exits the child process only
	}else if (pid < 0){
		//fork failure
		perror("kapish");
	}else{
		//parent process - fork has returned the pid of the child process
		/*wait for the child process to return or be stopped. A 0 will be put in
			status from the termination of the childlocklock process and the loop will exit*/
		do {
			signal(SIGINT, INThandler);
			wpid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	UNUSED(wpid);
	return 1;
}

/*for reading input from .kapishrc file - very similar to kapish_read_line*/
void kapishrc_init(void){
	FILE *fp;
	char *buffer = malloc(sizeof(char) * MAX_INPUT_LENGTH);
	int linenum_size = LINE_BUFFSIZE;
	char **lines = malloc(sizeof(char*) * linenum_size);
	char **args;
  int position = 0, linenum = 0;
  int ch, status;

	//set rcpath (from HOME env variable if available) - function in utilfunctions.c
	char *rcpath = set_rcpath();

	//open .kapishrc file
	fp = fopen(rcpath, "r");
	if(fp == NULL){
		perror("error while opening .kapishrc\n");
		return;
	}else{
		printf(".kapishrc found at %s\n", rcpath);
	}

  //error-handler for allocation failure
  if(!buffer){
 	 fprintf(stderr, "kapish: allocation of buffer error\n");
 	 free(buffer);
 	 exit(EXIT_FAILURE);
  }

	while((ch = fgetc(fp)) != EOF){
		 //once newline is read, replace with null character and return
	 	 //other wise add ch to buffer and make sure length has not been exceeded
	 	 if(ch == '\n'){
	 		 buffer[position] = '\0';
			 //alloc memory for a line, then copy buffer to that line
			 lines[linenum] = malloc(sizeof(char) * position);
			 strncpy(lines[linenum], buffer, position);
			 linenum++;
			 position = 0;
				 //increase lines array size if needed
			 if(linenum >= linenum_size){
				 linenum += TOKEN_BUFFSIZE;
				 lines = realloc(lines, sizeof(char*) * linenum_size);
				 if(!lines){
					 fprintf(stderr, "kapish: allocation of token array error\n");
					 free(lines);
					 free(buffer);
					 exit(EXIT_FAILURE);
				 }
			 }
	 	 } else {
	 		 buffer[position] = ch;
			 position++;
	 	 }

			 //input has exceeded max length and buffer will be changed to the ignore_cmd
		 if(position >= MAX_INPUT_LENGTH){
			 printf("kapish: input has exceeded maximum length and will be ignored\n");
			 buffer[position] = '\0';
			 printf("%s.kapishrc -%s %s\n", KGRN, KNRM, buffer);
			 lines[linenum++] = ignore_cmd; //add ignore_cmd to lines array
			 position = 0;
				 //increase lines array size if needed
			 if(linenum >= linenum_size){
				 linenum += TOKEN_BUFFSIZE;
				 lines = realloc(lines, sizeof(char*) * linenum_size);
				 if(!lines){
					 fprintf(stderr, "kapish: allocation of token array error\n");
					 free(lines);
					 free(buffer);
					 exit(EXIT_FAILURE);
				 }
			 }
		 }
	 }
	 fclose(fp);
	 //lines is now full of all the lines read from file

	 //loop for tokenizing all the lines and executing them
	 for(int i = 0; i < linenum; i++){
		 printf("%s.kapishrc -%s %s\n", KGRN, KNRM, lines[i]);
		 args = kapish_tokenize(lines[i]);
		 status = kapish_execute(args);
		 if(args != NULL){ free(args); }
		 if(status == 0){ break;}
	 }
	 free(buffer);
	 for(int c = 0; c < linenum; c++){
		 if(lines[c] != NULL && strncmp(lines[c], ignore_cmd, strlen(lines[c]))){
			 free(lines[c]);
		 }
	 }
	 free(lines);
	 free(rcpath);

	 if(status == 0){
		 exit(EXIT_SUCCESS);
	 }
}

/*for handling ctrl-c and preventing it from closing kapish*/
void INThandler(int sig){}


int main(int argc, char **argv){
  //load kapishrc file first
	kapishrc_init();

	signal(SIGINT, INThandler);
  //run command loop
  kapish_loop();

  //exit

  return EXIT_SUCCESS;
}
