#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEBUG 1

#define INSTRUCTIONS_FILE "instructions.txt"
#define PROCESSES_FOLDER "processes"
#define SCHEDULE_FILE "inputs/def8.txt" //"input.txt" // // "definition.txt"

#define MAX_CHAR_SIZE 100
#define NUM_PROCESSES 10

#define MAX_INSTRUCTIONS_PER_PROCESS 100
#define MAX_PROCESSES 100
#define MAX_PRIORITY 256

#define CONTEXT_SWITCH_TIME 10

char* get_next_element(char* str, int* index) {
    char* element = (char*)calloc(MAX_CHAR_SIZE, sizeof(char));
    int i = 0;

    while (str[*index] != ' ' && str[*index] != '\0' && str[*index] != '\n') {
        element[i] = str[*index];
        (*index)++;
        i++;
    }

    return element;
}

char** split(char* str) {
    char** tokens = (char**)calloc(MAX_CHAR_SIZE, sizeof(char*));
    int index = 0;
    int i = 0;

    while (str[index] != '\0') {
        tokens[i++] = get_next_element(str, &index);
        if (str[index] != '\0') {
            index++;
        }
    }

    return tokens;
}

#endif