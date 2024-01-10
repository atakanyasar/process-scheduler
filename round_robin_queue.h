#ifndef RR_QUEUE_H
#define RR_QUEUE_H

#include "util.h"
#include "process.h"

typedef struct {
    Process *processes[MAX_PROCESSES];
    int head;
    int tail;
    int size;
} RRQueue;

RRQueue rr_queue[MAX_PRIORITY];
RRQueue platinum_queue[MAX_PRIORITY];

void add_process_to_queue(Process *process) {
    RRQueue *queue = (process->type == PLATINUM ? &platinum_queue[process->priority] : &rr_queue[process->priority]);
    queue->processes[queue->tail] = process;
    queue->tail = (queue->tail + 1) % MAX_PROCESSES;
    queue->size++;
}

Process* get_next_process() {
    for (int i = MAX_PRIORITY - 1; i >= 0; i--) {
        RRQueue *queue = &platinum_queue[i];
        if (queue->size > 0) {
            Process *process = queue->processes[queue->head];
            return process;
        }
    }
    for (int i = MAX_PRIORITY - 1; i >= 0; i--) {
        RRQueue *queue = &rr_queue[i];
        if (queue->size > 0) {
            Process *process = queue->processes[queue->head];
            return process;
        }
    }
    return NULL;
}

void remove_process_from_queue(Process *process) {
    RRQueue *queue = (process->type == PLATINUM ? &platinum_queue[process->priority] : &rr_queue[process->priority]);
    queue->head = (queue->head + 1) % MAX_PROCESSES;
    queue->size--;
}

#endif