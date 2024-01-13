/*
    * This file contains the function declarations for the priority queue.
    * The priority queue is implemented as a standard binary heap.
    * The heap uses compare_process function to compare two processes.
    * The heap is used in the following functions:
        * push_to_process_queue
        * pop_from_process_queue
        * get_next_process
*/

#ifndef PRIORITY_QUEUE
#define PRIORITY_QUEUE

#include "util.h"
#include "process.h"

int swap(int i, int j);

int parent(int i);

void percolate_up(int i);

void percolate_down(int i);

void push_to_process_queue(Process* process);

Process* pop_from_process_queue();

Process* get_next_process();

#endif