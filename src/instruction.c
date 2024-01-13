#include "util.h"
#include "instruction.h"


Instruction* read_instructions(char* name) {
    Instruction* instructions = (Instruction*)calloc(MAX_INSTRUCTIONS_PER_PROCESS, sizeof(Instruction));
    int num_instructions = 0;
    FILE* file = fopen(INSTRUCTIONS_FILE, "r");
        
    if (file == NULL) {
        printf("Error opening file %s\n", INSTRUCTIONS_FILE);
        exit(1);
    }
    
    char* line = (char*)calloc(MAX_CHAR_SIZE, sizeof(char));

    while (fgets(line, MAX_CHAR_SIZE, file) != NULL) {
        char* instruction_name = strtok(line, " ");
        int duration = atoi(strtok(NULL, " "));
        
        strcpy(instructions[num_instructions].name, instruction_name);
        instructions[num_instructions].duration = duration;
        num_instructions++;
    }
    
    free(line);
    fclose(file);

    return instructions;
}

Instruction* get_instruction(char* name) {
    static Instruction* instructions = NULL;
    static int instructions_read = 0;

    if (instructions_read == 0) {
        instructions = read_instructions(name);
        instructions_read = 1;
    }

    for (int i = 0; i < MAX_INSTRUCTIONS_PER_PROCESS && instructions != NULL; i++) {
        if (strcmp(instructions[i].name, name) == 0) {
            return &instructions[i];
        }
    }

    return NULL;
}
