#include "priority_queue.h"
#include "util.h"
#include "process.h"

Process* process_heap[MAX_PROCESSES];
int size = 0;

int swap(int i, int j) {
    Process* temp = process_heap[i];
    process_heap[i] = process_heap[j];
    process_heap[j] = temp;
}

int parent(int i) {
    return (i - 1) / 2;
}

void percolate_up(int i) {
    while (compare_processes(process_heap[parent(i)], process_heap[i]) > 0) {
        swap(parent(i), i);
        i = parent(i);
    }
}
void percolate_down(int i) {

    while (1) {
        int left_child = 2 * i + 1;
        int right_child = 2 * i + 2;
        int smallest = i;
        
        if (left_child < size && compare_processes(process_heap[left_child], process_heap[smallest]) < 0) {
            smallest = left_child;
        }

        if (right_child < size && compare_processes(process_heap[right_child], process_heap[smallest]) < 0) {
            smallest = right_child;
        }

        if (smallest == i) {
            return;
        }

        swap(i, smallest);
        i = smallest;
    }

}

void push_to_process_queue(Process* process) {
    process_heap[size] = process;
    percolate_up(size++);
}

Process* pop_from_process_queue() {
    if (size == 0) {
        return NULL;
    }
    size--;
    swap(0, size);
    process_heap[size] = NULL;
    percolate_down(0);

}

Process* get_next_process() {
    if (size == 0) {
        return NULL;
    }
    return process_heap[0];
}
