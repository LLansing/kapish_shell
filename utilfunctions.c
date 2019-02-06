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
char *set_rcpath(void){
	int pathsize = MAX_RCPATH_LENGTH;
	char *rcpath = malloc(sizeof(char) * pathsize);
	char *rc_string = "/.kapishrc";

	if(getenv("HOME") != NULL){
		//reallocate space for rcpath if needed
		while(strlen(getenv("HOME")) >= pathsize - strlen(rc_string)){
			pathsize += MAX_RCPATH_LENGTH;
			rcpath = realloc(rcpath, sizeof(char) * pathsize);
		}
			//get HOME variable into rcpath and concatenate rc_string
		strncpy(rcpath, getenv("HOME"), strlen(getenv("HOME")));
		strncat(rcpath, rc_string, strlen(rc_string));

	}else{
		//if HOME not found, set rcpath to ".kapishrc"
		strncpy(rcpath, rc_string + 1, strlen(rc_string) - 1);
	}
	return rcpath;
}
