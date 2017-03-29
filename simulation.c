/*
 * COMP30024 Computer Systems
 * Semester 1 2017
 * Assignment 1
 *
 * Geoffrey Law (glaw@student.unimelb.edu.au)
 * 759218
 *
 * simulation.c
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "simulation.h"
#include "computer.h"
#include "list.h"

/* Simulate the memory management task */
void simulate(char *filename, char *algorithm_name, int memsize, int quantum) {
    initialize_computer(algorithm_name, memsize, quantum);

    computer_t *computer = get_instance();

    list_t *all_processes = load_processes(filename);
    int num_processes = len(all_processes);

    while (computer->cpu->num_completed_process < num_processes) {
        create_process(&all_processes);
        int event = observe_event();
        if (event) {
            (computer->cpu->swap)();
            (computer->cpu->schedule)(event);
            // printf("queue:\n");
            // print_list(print_process, stdout, computer->cpu->process_queue->head);
            // printf("event: %d\nrunning: %d\nsegments:\n", event, computer->cpu->running_process->process_id);
            // print_list(print_segment, stdout, computer->memory->segment_list->head);
            // printf("------\n\n");
        }
        cpu_step();
        (*time())++;
    }

    fprintf(stdout, "time %d, simulation finished.\n", *time() - 1);
}

/* Get clock time from the singleton */
int *time() {
    static int *time = NULL;
    if (time == NULL) {
        time = (int*)malloc(sizeof(int));
        *time = 0;
    }
    return time;
}

/* Observe and return an event */
int observe_event() {
    computer_t *computer = get_instance();

    int event = NONE;

    /* E1 */
    node_t *segment_node = computer->memory->segment_list->head;
    bool empty_flag = true;
    while (segment_node != NULL) {
        segment_t *segment = (segment_t*) segment_node->data;
        if (segment->process != NULL) {
            empty_flag = false;
            break;
        }
        segment_node = segment_node->next;
    }
    if (empty_flag) {
        event = E1;
    }

    /* E2, E3 */
    if (computer->cpu->running_process != NULL) {
        if (computer->cpu->running_process->job_time == 0) {
            event = E3;
        } else if (computer->cpu->running_time == computer->cpu->quantum) {
            event = E2;
        }
    }

    return event;
}

void cpu_step() {
    computer_t *computer = get_instance();

    if (computer->cpu->running_process != NULL) {
        if (computer->cpu->running_time < computer->cpu->quantum
            && computer->cpu->running_process->job_time != 0) {
            computer->cpu->running_time += 1;
            computer->cpu->running_process->job_time -= 1;
        }
        if (computer->cpu->running_process->job_time == 0) {
            del_memory_process(computer->cpu->running_process);
        }
    }
}

/* Add created process to disk */
void create_process(list_t **all_processes) {
    computer_t *computer = get_instance();

    node_t *process_node = (*all_processes)->head;

    while (process_node != NULL) {
        process_t *process = (process_t*) process_node->data;
        if (process->time_created == *time()) {
            process_t *data = pop(all_processes);
            insert_at_tail(data, &(computer->disk->process_list));
        }
        process_node = process_node->next;
    }
}

/* Load processes from given input file */
list_t *load_processes(char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Fail to open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    list_t *process_list = NULL;
    int time_created, process_id, memory_size, job_time;

    while (fscanf(f, "%d %d %d %d\n", &time_created, &process_id, &memory_size, &job_time) == NUM_ENTRIES) {
        process_t *process = (process_t*)malloc(sizeof(process_t));
        process->time_created = time_created;
        process->process_id = process_id;
        process->memory_size = memory_size;
        process->job_time = job_time;
        process->time_placed_on_disk = time_created;
        process->time_placed_on_memory = -1;
        insert_at_tail(process, &process_list);
    }

    return process_list;
}
