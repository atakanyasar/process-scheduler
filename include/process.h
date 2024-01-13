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

/*
 * Status of the process:
    * - WAITING: process is waiting for arrival time
    * - READY: process is in the priority queue waiting for execution
    * - RUNNING: process is currently running in CPU
    * - FINISHED: process has finished execution
 */
typedef enum ProcessStatus {
    WAITING,
    READY,
    RUNNING,
    FINISHED
} ProcessStatus;

/*
    * Processes consist of:
        * - name: name of the process
        * - instructions: instructions of the process
        * - num_instructions: number of instructions of the process
        * - priority: priority of the process
        * - arrival_time: arrival time of the process
        * - end_time: end time of the process
        * - queue_entry_time: time when the process entered the priority queue
        * - type: type of the process
        * - status: status of the process
        * - last_instruction: index of the last instruction executed
        * - quantum_burst_time: time spent in the current quantum
        * - quantum_count: number of quanta spent in the current queue
*/
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

/*
 * Reads the process with the given name from the file and returns it.
 */
Process* read_process(char* name);

/*
 * Searches the process with the given name in local process list.
 * If it is not found, reads the process from the file and adds it to the local list.
 * Returns the process in the list.
 */
Process* get_process(char* process_name);

/*
 * Returns the total burst time of the process.
 */
int get_burst_time(Process* process);

/*
 * Returns the turnaround time of the process after finished.
 */
int get_turnaround_time(Process* process);

/*
 * Returns the waiting time of the process after finished.
 */
int get_waiting_time(Process* process);

/*
 * Returns the time quantum of the process according to its type.
 */
int get_time_quantum(Process* process);

/*
 * Checks if the process has a promotion.
    * If the process is PLATINUM, it is not promoted.
    * If the process is GOLD, it is promoted to PLATINUM if its quantum count reaches to 5.
    * If the process is SILVER, it is promoted to GOLD if its quantum count reaches to 3.
 */
void check_promotions(Process* curr_process);

/*
 * The comparison function prioritizes platinum processes, followed by higher-priority processes, earlier arrival times, and process names.
    * - Returns -1 if A is higher priority than B, 1 if B is higher priority than A, and 0 if they are equal
    * - Platinum processes are always higher priority than non-platinum processes
    * - The process that have a greater valued priority is higher priority than the process that have a lower valued priority.
    * - If the processes have the same priority, the process that arrived at queue earlier is higher priority than the process that arrived later.
    * - If the processes have the same priority and arrived at the same time, the process that has a name that comes earlier in the alphabet is higher priority than the process that has a name that comes later in the alphabet.
    * - If the processes have the same priority, arrived at the same time, and have the same name, they are equal priority.
*/
int compare_processes(Process* A, Process* B);

#endif