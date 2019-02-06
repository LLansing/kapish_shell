#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "utilfunctions.h"

#define MAX_RCPATH_LENGTH 100 //max number of chars taken for rcpath

char *get_time_str(void){
	time_t curtime;
	struct tm *loc_time;

	//get current time of the system
	curtime = time(NULL);

	//convert current time to local time
	loc_time = localtime (&curtime);

	char *t_string = asctime(loc_time);
	t_string[strlen(t_string) - 1] = '\0';
	// return date and time in standard format
	return t_string;
}

/*Returns rcpath set to $(HOME)/.kapishrc where $(HOME) is the HOME env variable
	If HOME is not found*/
char *set_rcpath(char** rcpath){
	int pathsize = MAX_RCPATH_LENGTH;
	char *rc_string = "/.kapishrc";

	strncpy(*rcpath, rc_string + 1, strlen(rc_string) - 2);
	printf("rc_path default: %s\n", rc_string);

	if(getenv("HOME") != NULL){
		while(strlen(getenv("HOME")) >= pathsize - strlen(rc_string)){
			pathsize += MAX_RCPATH_LENGTH;
			rcpath = realloc(*rcpath, sizeof(char) * pathsize);
		}
		strncpy(*rcpath, getenv("HOME"), strlen(getenv("HOME")));
		strncat(*rcpath, rc_string, strlen(rc_string));
		printf("rc_path HOME: %s\n", rc_string);
	}else{
		strncpy(*rcpath, rc_string + 1, strlen(rc_string) - 2);
	}
	return *rcpath;
}
