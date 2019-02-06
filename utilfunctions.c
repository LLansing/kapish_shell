#include <stdio.h>
#include <time.h>
#include <string.h>
#include "utilfunctions.h"

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
