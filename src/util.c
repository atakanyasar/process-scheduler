#include "util.h"

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