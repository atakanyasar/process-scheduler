#include <stdio.h>
#include "scheduler.h"

int main() {

    // simulate the processes with scheduler
    Scheduler* scheduler = read_processes();
    simulate(scheduler); 

    // if average waiting time is an integer, print it as an integer without decimal places
    if (get_average_waiting_time(scheduler) == (int)get_average_waiting_time(scheduler)) {
        printf("%d\n", (int)get_average_waiting_time(scheduler));
    } else {
        printf("%.1f\n", get_average_waiting_time(scheduler));
    }

    // if average turnaround time is an integer, print it as an integer without decimal places
    if (get_average_turnaround_time(scheduler) == (int)get_average_turnaround_time(scheduler)) {
        printf("%d\n", (int)get_average_turnaround_time(scheduler));
    } else {
        printf("%.1f\n", get_average_turnaround_time(scheduler));
    }

    return 0;
}