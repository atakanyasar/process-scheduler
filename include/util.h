/*
 * This file contains the utility functions for input parsing.
 * Also contains the constants used in the program.
 */
 

#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEBUG 0                                 // set to 1 to enable debug mode

#define INSTRUCTIONS_FILE "instructions.txt"    // file containing the instructions
#define PROCESSES_FOLDER "processes"            // folder containing the processes
#define SCHEDULE_FILE "definition.txt"          // file containing the schedule definition

#define MAX_CHAR_SIZE 100                       // maximum size of a string
#define NUM_PROCESSES 10                        // number of processes

#define MAX_INSTRUCTIONS_PER_PROCESS 30         // maximum number of instructions per process
#define MAX_PROCESSES 20                        // maximum number of processes
#define MAX_PRIORITY 256                        // maximum priority of a process

#define CONTEXT_SWITCH_TIME 10

char* get_next_element(char* str, int* index);

char** split(char* str);

#endif