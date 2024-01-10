#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "util.h"
#include "instruction.h"
#include "process.h"
#include "round_robin_queue.h"

typedef struct {
    Process processes[MAX_PROCESSES];
    int num_processes;
    int current_time;
    Process* current_process;
} Scheduler;

double get_average_turnaround_time(Scheduler* scheduler) {
    long long int total_turnaround_time = 0;
    for (int i = 0; i < scheduler->num_processes; i++) {
        total_turnaround_time += get_turnaround_time(&scheduler->processes[i]);
    }
    return (double)total_turnaround_time / (double)scheduler->num_processes;
}

double get_average_waiting_time(Scheduler* scheduler) {
    long long int total_waiting_time = 0;
    for (int i = 0; i < scheduler->num_processes; i++) {
        total_waiting_time += get_waiting_time(&scheduler->processes[i]);
    }
    return (double)total_waiting_time / (double)scheduler->num_processes;
}

Scheduler* read_processes() {
    FILE* file = fopen(SCHEDULE_FILE, "r");
    
    if (file == NULL) {
        printf("Error opening file %s\n", SCHEDULE_FILE);
        exit(1);
    }
    
    Scheduler* scheduler = calloc(1, sizeof(Scheduler));
    
    char* line = calloc(MAX_CHAR_SIZE, sizeof(char));
    
    while (fgets(line, MAX_CHAR_SIZE, file) != NULL) {

        char** tokens = split(line);
        Process* process = get_process(tokens[0]);
        process->priority = atoi(tokens[1]);
        process->arrival_time = atoi(tokens[2]);
        char* process_type = tokens[3];

        if (strcmp(process_type, "PLATINUM") == 0) {
            process->type = PLATINUM;
        } else if (strcmp(process_type, "GOLD") == 0) {
            process->type = GOLD;
        } else if (strcmp(process_type, "SILVER") == 0) {
            process->type = SILVER;
        } else {
            printf("Error: process type %s not found\n", process_type);
            exit(1);
        }

        scheduler->processes[scheduler->num_processes++] = *process;

        for (int i = 0; tokens != NULL && tokens[i] != NULL; i++) {
            free(tokens[i]);
        }
        free(tokens);

    }

    free(line);
    fclose(file);

    return scheduler;
}

Process* jump_to_next_process(Scheduler* scheduler) {
    int min_arrival_time = 1e9;
    Process* next_process = NULL;

    for (int i = 0; i < scheduler->num_processes; i++) {
        if (scheduler->processes[i].status == WAITING && scheduler->processes[i].arrival_time >= scheduler->current_time) {
            if (min_arrival_time > scheduler->processes[i].arrival_time) {
                min_arrival_time = scheduler->processes[i].arrival_time;
                next_process = &scheduler->processes[i];
            }
        }
    }

    if (next_process != NULL) {
        scheduler->current_time = next_process->arrival_time;
    }

    return next_process;
}

void handle_arrived_processes(Scheduler* scheduler) {
    Process* arrived_process = NULL;
    do {
        arrived_process = NULL;

        for (int i = 0; i < scheduler->num_processes; i++) {
            Process* process = &scheduler->processes[i];
            if (process->arrival_time <= scheduler->current_time && process->status == WAITING) {
                if (arrived_process == NULL || process->arrival_time < arrived_process->arrival_time || (process->arrival_time == arrived_process->arrival_time && strcmp(process->name, arrived_process->name) < 0)) {
                    arrived_process = process;
                }
            }
        }

        if (arrived_process == NULL) {
            break;
        }

        arrived_process->status = READY;
        add_process_to_queue(arrived_process);

    } while (arrived_process != NULL);
    
}

int all_processes_finished(Scheduler* scheduler) {
    for (int i = 0; i < scheduler->num_processes; i++) {
        if (scheduler->processes[i].status != FINISHED) {
            return 0;
        }
    }
    return 1;
}

int should_preempt(Scheduler* scheduler, Process* process) {
    Process* curr_process = scheduler->current_process;

    if (process == NULL) {
        return 0;
    }
    if (curr_process == NULL) {
        return 1;
    }
    if (curr_process->type == PLATINUM) {
        return 0;
    }
    if (process->type == PLATINUM) {
        return 1;
    }
    if (curr_process->priority < process->priority) {
        return 1;
    }
    if (curr_process->priority > process->priority) {
        return 0;
    }
    if (curr_process->quantum_burst_time < get_time_quantum(curr_process)) {
        return 0;
    }
    return 1;
}

Process* context_switch(Scheduler* scheduler, Process* switched) {
    Process* curr_process = scheduler->current_process;

    scheduler->current_process = switched;

    switched->status = RUNNING;
    switched->quantum_burst_time = 0;
    remove_process_from_queue(switched);
    
#if DEBUG == 1
    fprintf(stderr, "time %d: context_switch (%s, %s), duration=10, finished_at=%d\n", 
        scheduler->current_time, switched->name, curr_process == NULL ? "NULL" : curr_process->name, scheduler->current_time + CONTEXT_SWITCH_TIME);
#endif

    scheduler->current_time += CONTEXT_SWITCH_TIME;

    if (curr_process != NULL) {
        curr_process->status = READY;
        add_process_to_queue(curr_process);
    }

    return curr_process;
}

Process* simulate(Scheduler* scheduler) {


    while (all_processes_finished(scheduler) == 0) {
        jump_to_next_process(scheduler);
        handle_arrived_processes(scheduler);

        Process* curr_process = NULL;
        while ((curr_process = get_next_process(scheduler)) != NULL || scheduler->current_process != NULL) {
            
            if (should_preempt(scheduler, curr_process)) {
                context_switch(scheduler, curr_process);
            } 
            else {
                curr_process = scheduler->current_process;
            }

#if DEBUG == 1
            fprintf(stderr, "time %d: %s in CPU, line=%d, %s, duration=%d, finished_at=%d\n", scheduler->current_time, curr_process->name, curr_process->last_instruction, 
                curr_process->instructions[curr_process->last_instruction].name, curr_process->instructions[curr_process->last_instruction].duration, 
                scheduler->current_time + curr_process->instructions[curr_process->last_instruction].duration);
#endif

            scheduler->current_time += curr_process->instructions[curr_process->last_instruction].duration;
            curr_process->quantum_burst_time += curr_process->instructions[curr_process->last_instruction].duration;

            if (curr_process->type == SILVER && curr_process->quantum_burst_time >= 3 * get_time_quantum(curr_process)) {
                curr_process->quantum_burst_time = 0;
                curr_process->type = GOLD;
            }
            if (curr_process->type == GOLD && curr_process->quantum_burst_time >= 5 * get_time_quantum(curr_process)) {
                curr_process->quantum_burst_time = 0;
                curr_process->type = PLATINUM;
            }

            if (strcmp(curr_process->instructions[curr_process->last_instruction].name, "exit") == 0) {
                curr_process->status = FINISHED;
                curr_process->end_time = scheduler->current_time;
                scheduler->current_process = NULL;
            }
            curr_process->last_instruction++;
            
            handle_arrived_processes(scheduler);
        } 
    }
}
int main() {

    Scheduler* scheduler = read_processes();
    simulate(scheduler);
    
    // print 1 significant digit after the decimal point
    // but if the number is an integer, don't print the decimal point

    if (get_average_waiting_time(scheduler) == (int)get_average_waiting_time(scheduler)) {
        printf("%d\n", (int)get_average_waiting_time(scheduler));
    } else {
        printf("%.1f\n", get_average_waiting_time(scheduler));
    }

    if (get_average_turnaround_time(scheduler) == (int)get_average_turnaround_time(scheduler)) {
        printf("%d\n", (int)get_average_turnaround_time(scheduler));
    } else {
        printf("%.1f\n", get_average_turnaround_time(scheduler));
    }

    //printf("%.1f\n", get_average_waiting_time(scheduler));
    //printf("%.1f\n", get_average_turnaround_time(scheduler));


    return 0;
}