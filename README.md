--== Kapish Shell - Assignment 1 - CSC360 ==--
--== Lance Lansing - Vxxxxxxxx - Jan 2019 ==--

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
    - When a 2nd (or deeper) level of kapish is ran, CTRL-C fails to close any
			of them (as intended), but multiple prompts are printed after catching
			CTRL-C

COLLABORATORS:
		- Discussed project with some CSC360 students
