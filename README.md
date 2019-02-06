# --== Kapish Shell - Assignment 1 - CSC360 ==--
## --== Lance Lansing       -       Jan 2019 ==--

DESCRIPTION:

Command line shell called kapish with the following functions implemented

				- Reads .kapishrc file from home directory (if HOME env variable is available) and executes commands within. Then, if no exit command was given, the interactive kapish command line is opened

				- builtin functions:

					- cd - change directory, go to home if no directory specified

					- exit - return 0, ending the command loop

					- setenv - sets a given environment variable to a given value creating it if
					 it does not exist

					- unsetenv - deletes given environment variable

				- reads in commands and executes them using PATH environment variable

				- catches ctrl-c signal, preventing it from killing kapish

				- ctrl-d signal kills kapish at command prompt


SOURCES:

  	- 'Tutorial - Write a Shell in C', Stephen Brennan, January 16th, 2015
      https://brennan.io/2015/01/16/write-a-shell-in-c/


    - Makefile tutorial http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
        as well as makefile code Zehui Zheng showed in tutorial


    - 'How To Eliminate Unused Variable Warnings in GCC', jdfulmer, September 23,
       2014
         https://www.joedog.org/2014/09/23/how-to-eliminate-unused-variable-warnings-in-gcc/


    - Signal handling code shown by Zehui Zheng in tutorial

KNOWN BUGS:

FIXED BUGS:

		- When a 2nd (or deeper) level of kapish is ran, CTRL-C fails to close any
			of them (as intended), but multiple prompts are printed after catching
			CTRL-C

COLLABORATORS:
		- Discussed project with some CSC360 students
