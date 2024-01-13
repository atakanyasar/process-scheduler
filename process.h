#ifndef PROCESS_H
#define PROCESS_H

#include "util.h"
#include "instruction.h"

typedef enum ProcessType {
    PLATINUM,
    GOLD,
    SILVER
} ProcessType;

typedef enum TimeQuantum {
    PLATINUM_TIME_QUANTUM = 100000000, //120,
    GOLD_TIME_QUANTUM = 120,
    SILVER_TIME_QUANTUM = 80
} TimeQuantum;

typedef enum ProcessStatus {
    WAITING,
    READY,
    IN_QUEUE,
    RUNNING,
    FINISHED
} ProcessStatus;

typedef struct {
    char name[MAX_CHAR_SIZE];
    Instruction instructions[MAX_INSTRUCTIONS_PER_PROCESS];
    int num_instructions;
    int priority;
    int arrival_time;
    int end_time;
    int queue_entry_time;
    ProcessType type;
    ProcessStatus status;
    int last_instruction;
    int quantum_burst_time;
    int quantum_count;
} Process;

Process* read_process(char* name) {
    char* process_file = calloc(MAX_CHAR_SIZE, sizeof(char));
    sprintf(process_file, "%s/%s.txt", PROCESSES_FOLDER, name);
    
    FILE* file = fopen(process_file, "r");
    
    if (file == NULL) {
        printf("Error opening file %s\n", process_file);
        exit(1);
    }
    
    Process* process = calloc(1, sizeof(Process));
    strcpy(process->name, name);
    
    char* line = calloc(MAX_CHAR_SIZE, sizeof(char));
    
    while (fgets(line, MAX_CHAR_SIZE, file) != NULL) {
        
        char* instruction_name = strtok(line, "\n");
        Instruction* instruction = get_instruction(instruction_name);

        if (instruction == NULL) {
            printf("Error: instruction %s not found\n", line);
            exit(1);
        }
        
        process->instructions[process->num_instructions++] = *instruction;
    }

    free(line);
    free(process_file);
    fclose(file);

    return process;
}

int get_burst_time(Process* process) {
    int burst_time = 0;
    for (int i = 0; i < process->num_instructions; i++) {
        burst_time += process->instructions[i].duration;
    }
    return burst_time;
}

int get_turnaround_time(Process* process) {
    return process->end_time - process->arrival_time;
}

int get_waiting_time(Process* process) {
    return get_turnaround_time(process) - get_burst_time(process);
}

int get_time_quantum(Process* process) {
    switch (process->type) {
        case PLATINUM:
            return PLATINUM_TIME_QUANTUM;
        case GOLD:
            return GOLD_TIME_QUANTUM;
        case SILVER:
            return SILVER_TIME_QUANTUM;
        default:
            printf("Error: process type %d not found\n", process->type);
            exit(1);
    }
}

Process* get_process(char* process_name) {
    static Process* processes = NULL;
    static int num_processes = 0;
    
    for (int i = 0; i < num_processes && processes != NULL; i++) {
        if (strcmp(processes[i].name, process_name) == 0) {
            return &processes[i];
        }
    }

    if (processes == NULL) {
        processes = calloc(MAX_PROCESSES, sizeof(Process));
    }
    Process* process = read_process(process_name);

    if (process == NULL) {
        printf("Error: process %s not found\n", process_name);
        exit(1);
    }
    
    processes[num_processes++] = *process;
    return process;
}

void check_promotions(Process* curr_process) {
    if (curr_process->type == SILVER && curr_process->quantum_count == 3) {
        curr_process->quantum_count = 0;
        curr_process->type = GOLD;
#if DEBUG == 1
        fprintf(stderr, "time ***: %s promoted to GOLD\n", curr_process->name);
#endif
    }
    if (curr_process->type == GOLD && curr_process->quantum_count == 5) {
        curr_process->quantum_count = 0;
        curr_process->type = PLATINUM;
#if DEBUG == 1
        fprintf(stderr, "time ***: %s promoted to PLATINUM\n", curr_process->name);
#endif
    }
}

int compare_processes(Process* A, Process* B) {
    if (A->type == PLATINUM && B->type != PLATINUM) {
        return -1;
    }
    if (A->type != PLATINUM && B->type == PLATINUM) {
        return 1;
    }
    if (A->priority > B->priority) {
        return -1;
    }
    if (A->priority < B->priority) {
        return 1;
    }
    if (A->queue_entry_time < B->queue_entry_time) {
        return -1;
    }
    if (A->queue_entry_time > B->queue_entry_time) {
        return 1;
    }
    if (strcmp(A->name, B->name) < 0) {
        return -1;
    }
    if (strcmp(A->name, B->name) > 0) {
        return 1;
    }
    return 0;
}

#endif