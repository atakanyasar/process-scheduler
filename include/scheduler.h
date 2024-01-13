#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "util.h"
#include "instruction.h"
#include "process.h"
#include "priority_queue.h"

/* Scheduler consists of:
 * - processes: array of processes
 * - num_processes: number of processes
 * - current_time: current time of the scheduler
 * - current_process: process currently running in the CPU
 * - last_executed_process: last process executed in the CPU
 */
typedef struct {
    Process processes[MAX_PROCESSES];
    int num_processes;
    int current_time;
    Process* current_process;
    Process* last_executed_process;
} Scheduler;

/*
 * Returns the average turnaround time of the processes in the scheduler.
 */
double get_average_turnaround_time(Scheduler* scheduler);

/*
 * Returns the average waiting time of the processes in the scheduler.
 */
double get_average_waiting_time(Scheduler* scheduler);

/*
 * Reads the processes from the definition.txt file.
 */
Scheduler* read_processes();

/*
 * Checks if the priority queue is empty and the CPU is idle. 
 * If so, then the current time jumps to the arrival time of the earlier waiting process.
 */
Process* jump_to_next_process(Scheduler* scheduler);

/*
 * Checks if there are any processes that have arrived at the current time.
 * If so, then they are added to the priority queue.
 */
void handle_arrived_processes(Scheduler* scheduler);

/*
 * If all processes have finished execution, return 1
 * Else returns 0
 */
int all_processes_finished(Scheduler* scheduler);

/*
 * If the candidate process should preempt the current process, return 1
 * Else returns 0
 * No preemption conditions:
    * - the candidate process is NULL.
    * - the current process is platinum.
* Preemption conditions:
    * - the current process is NULL.
    * - the candidate process is platinum and the current process is not.
    * - the candidate process has a higher priority than the current non-platinum process.
    * - the total consecutive burst time of the current process has exceed its time quantum.
 */
int should_preempt(Scheduler* scheduler, Process* process);

/*
 * Releasing a process from the CPU involves adding the process back to the priority queue and removing it from the CPU. 
 * The quantum count of the released process is increased, and the scheduler checks for potential promotion. 
 * This function is called in 2 cases:
 * 1. After exceeding quantum time
 * 2. The next candidate process in priority queue should preempt the running process
 */
void release_process(Scheduler* scheduler);

/*
 * Pulls the next process from the priority queue and adds it to the CPU.
 * Also, if the process is different from the last executed process, then the time is incremented by 10.
 */
void context_switch(Scheduler* scheduler);

/*
 * Simulates the scheduler by running the processes in the priority queue.
 * The scheduler runs until all processes have finished execution.
 * At each iteration of the loop:
 * - The scheduler jumps to the next process.
 * A nested loop is used to simulate the execution of the process in the CPU.
 * The nested loop runs until there is no process in the CPU and the priority queue is empty.
 * In the nested loop: 
 * - The scheduler handles arrived processes.
 * - the scheduler checks if the current process should be preempted.
 * - If so, then the current process is released and the next process is added to the CPU with context switch.
 * - The time increases by the burst time of the current instruction.
 * - If the current instruction is the last instruction of the process, then the process is terminated.
 * - If the process exceeds its time quantum, then it is released.
 */
void simulate(Scheduler* scheduler);
