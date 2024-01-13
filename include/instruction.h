

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "util.h"

/*
 * Instructions consist of:
    * - name: name of the instruction
    * - duration: duration of the instruction
 */
typedef struct {
    char name[MAX_CHAR_SIZE];
    int duration;
} Instruction;

/*
 * Reads the instructions from the file and returns them as an array.
 */
Instruction* read_instructions(char* name);

/*
 * Searches the instruction  with the given name in local instruction list.
 * If it is not found, reads the instructions from the file and adds it to the local list.
 * Returns the instruction in the list.
 */
Instruction* get_instruction(char* name);


#endif