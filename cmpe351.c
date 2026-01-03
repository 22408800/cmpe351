/* This is code developed by <Presley Ebikari Dein-Aboh> */
#include "cmpe351.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Process {
    int burst_time;
    int priority;
    int arrival_time;
    int queue_id;
    int process_id;
    int waiting_time;
    int completion_time;
    struct Process* next;
} Process;

typedef struct Queue {
    int queue_id;
    Process* processes;
    int process_count;
    struct Queue* next;
} Queue;

// Function prototypes
Process* create_process(int burst, int priority, int arrival, int queue_id, int pid);
void add_process_to_list(Process** head, Process* new_process);
Queue* create_queue(int queue_id);
void add_queue(Queue** head, Queue* new_queue);
Queue* find_or_create_queue(Queue** head, int queue_id);
void read_input_file(const char* filename, Queue** queues);
void fcfs_scheduling(Process* processes, int count);
void sjf_scheduling(Process* processes, int count);
void priority_scheduling(Process* processes, int count);
void calculate_waiting_times(Queue* queues, FILE* output_file);
void free_processes(Process* head);
void free_queues(Queue* head);
Process* copy_process_list(Process* head, int* count);

Process* create_process(int burst, int priority, int arrival, int queue_id, int pid) {
    Process* p = (Process*)malloc(sizeof(Process));
    if (!p) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    p->burst_time = burst;
    p->priority = priority;
    p->arrival_time = arrival;
    p->queue_id = queue_id;
    p->process_id = pid;
    p->waiting_time = 0;
    p->completion_time = 0;
    p->next = NULL;
    return p;
}

void add_process_to_list(Process** head, Process* new_process) {
    if (*head == NULL) {
        *head = new_process;
    } else {
        Process* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_process;
    }
}

Queue* create_queue(int queue_id) {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (!q) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    q->queue_id = queue_id;
    q->processes = NULL;
    q->process_count = 0;
    q->next = NULL;
    return q;
}

void add_queue(Queue** head, Queue* new_queue) {
    if (*head == NULL) {
        *head = new_queue;
    } else {
        Queue* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_queue;
    }
}

Queue* find_or_create_queue(Queue** head, int queue_id) {
    Queue* current = *head;
    while (current != NULL) {
        if (current->queue_id == queue_id) {
            return current;
        }
        current = current->next;
    }
    
    Queue* new_queue = create_queue(queue_id);
    add_queue(head, new_queue);
    return new_queue;
}

void read_input_file(const char* filename, Queue** queues) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error opening input file: %s\n", filename);
        exit(1);
    }
    
    int burst, priority, arrival, queue_id;
    char line[1024];
    
    while (fgets(line, sizeof(line), file)) {
        // Skip empty lines
        if (line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        
        if (sscanf(line, "%d:%d:%d:%d", &burst, &priority, &arrival, &queue_id) == 4) {
            Queue* q = find_or_create_queue(queues, queue_id);
            Process* p = create_process(burst, priority, arrival, queue_id, q->process_count);
            add_process_to_list(&(q->processes), p);
            q->process_count++;
        }
    }
    
    fclose(file);
}

Process* copy_process_list(Process* head, int* count) {
    Process* new_head = NULL;
    Process* current = head;
    *count = 0;
    
    while (current != NULL) {
        Process* p = create_process(current->burst_time, current->priority, 
                                    current->arrival_time, current->queue_id, 
                                    current->process_id);
        add_process_to_list(&new_head, p);
        (*count)++;
        current = current->next;
    }
    
    return new_head;
}

void fcfs_scheduling(Process* processes, int count) {
    if (processes == NULL || count == 0) return;
    
    int current_time = 0;
    Process* current = processes;
    
    while (current != NULL) {
        if (current_time < current->arrival_time) {
            current_time = current->arrival_time;
        }
        
        current->waiting_time = current_time - current->arrival_time;
        current->completion_time = current_time + current->burst_time;
        current_time = current->completion_time;
        
        current = current->next;
    }
}

void sjf_scheduling(Process* processes, int count) {
    if (processes == NULL || count == 0) return;
    
    Process** proc_array = (Process**)malloc(count * sizeof(Process*));
    if (!proc_array) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    Process* current = processes;
    int idx = 0;
    while (current != NULL) {
        proc_array[idx++] = current;
        current = current->next;
    }
    
    int* completed = (int*)calloc(count, sizeof(int));
    if (!completed) {
        free(proc_array);
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    int current_time = 0;
    int completed_count = 0;
    
    while (completed_count < count) {
        int shortest_idx = -1;
        int shortest_burst = 2147483647;
        
        for (int i = 0; i < count; i++) {
            if (!completed[i] && proc_array[i]->arrival_time <= current_time) {
                if (proc_array[i]->burst_time < shortest_burst) {
                    shortest_burst = proc_array[i]->burst_time;
                    shortest_idx = i;
                }
            }
        }
        
        if (shortest_idx == -1) {
            current_time++;
        } else {
            Process* p = proc_array[shortest_idx];
            p->waiting_time = current_time - p->arrival_time;
            current_time += p->burst_time;
            p->completion_time = current_time;
            completed[shortest_idx] = 1;
            completed_count++;
        }
    }
    
    free(proc_array);
    free(completed);
}
