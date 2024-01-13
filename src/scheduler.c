#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "scheduler.h"
#include "util.h"
#include "instruction.h"
#include "process.h"
#include "priority_queue.h"

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
        process->arrival_time = process->queue_entry_time = atoi(tokens[2]);
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

    for (int i = 0; i < scheduler->num_processes; i++) {
        Process* process = &scheduler->processes[i];
        if (process->status != WAITING) {
            continue;
        }
        if (process->arrival_time <= scheduler->current_time) {
            process->status = READY;
            process->queue_entry_time = process->arrival_time;
            push_to_process_queue(process);
        }
    }
    
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

void release_process(Scheduler* scheduler) {
    Process* curr_process = scheduler->current_process;

    scheduler->current_process = NULL;
    if (curr_process == NULL) {
        return;
    }

    curr_process->status = READY;
    curr_process->queue_entry_time = scheduler->current_time;
    curr_process->quantum_count++;
    curr_process->quantum_burst_time = 0;
    check_promotions(curr_process);
    push_to_process_queue(curr_process);

}

void context_switch(Scheduler* scheduler) {
    Process* switched = get_next_process();

    if (switched == NULL) {
        return;
    }

#if DEBUG == 1
    if (scheduler->last_executed_process != switched)
        fprintf(stderr, "time %d: context_switch (%s), duration=10, finished_at=%d\n", 
            scheduler->current_time, 
            switched->name, 
            scheduler->current_time + CONTEXT_SWITCH_TIME);
    else
        fprintf(stderr, "time %d: %s quantum_count=%d\n", scheduler->current_time, switched->name, switched->quantum_count);
#endif

    switched->status = RUNNING;
    switched->quantum_burst_time = 0;
    pop_from_process_queue();
    scheduler->current_process = switched;

    if (scheduler->last_executed_process != scheduler->current_process) {
        scheduler->current_time += CONTEXT_SWITCH_TIME;
    }

}

void simulate(Scheduler* scheduler) {

    while (all_processes_finished(scheduler) == 0) {
        jump_to_next_process(scheduler);

        while (1) {

            handle_arrived_processes(scheduler);

            if (should_preempt(scheduler, get_next_process())) {
                release_process(scheduler);
                context_switch(scheduler);
            } 
            
            Process* curr_process = scheduler->current_process;

            if (curr_process == NULL) {
                break;
            }

#if DEBUG == 1
            fprintf(stderr, "time %d: %s in CPU, line=%d, %s, duration=%d, quantum_count=%d, finished_at=%d\n", 
                scheduler->current_time, 
                curr_process->name, 
                curr_process->last_instruction, 
                curr_process->instructions[curr_process->last_instruction].name, 
                curr_process->instructions[curr_process->last_instruction].duration, 
                curr_process->quantum_count + (curr_process->quantum_burst_time >= get_time_quantum(curr_process) ? 1 : 0),
                scheduler->current_time + curr_process->instructions[curr_process->last_instruction].duration);
#endif

            scheduler->current_time += curr_process->instructions[curr_process->last_instruction].duration;
            curr_process->quantum_burst_time += curr_process->instructions[curr_process->last_instruction].duration;

            scheduler->last_executed_process = curr_process;
            
            if (strcmp(curr_process->instructions[curr_process->last_instruction++].name, "exit") == 0) {
                curr_process->status = FINISHED;
                curr_process->end_time = scheduler->current_time;
                scheduler->current_process = NULL;
                continue;
            }

            if (curr_process->quantum_burst_time >= get_time_quantum(curr_process)) {
                release_process(scheduler);
            }

        } 
    }
}
