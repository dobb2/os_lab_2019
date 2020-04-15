#ifndef OS_LAB_3_QUEUE_H
#define OS_LAB_3_QUEUE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct QueueItem queue_item;
struct QueueItem {
    struct QueueItem* next;
    struct QueueItem* prev;
    int value;
};

typedef struct Queue queue;
struct Queue {
    queue_item* head;
    queue_item* tail;
    int size;
};

void q_init(queue* q);
int q_top(const queue* q);
int q_pop(queue* q);
int q_size(const queue* q);
void q_push(queue* q, const int arrray);
bool q_empty(const queue* q);
void q_destroy(queue* q);
void q_print(const queue* q);

#endif //OS_LAB_3_QUEUE_H