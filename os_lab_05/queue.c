#include "queue.h"

void q_init(queue* q) {
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

int q_top(const queue *q) {
    return q->head->value;
}

int q_pop(queue* q) {

    int temp = q_top(q);
    queue_item* ptr_to_free = q->head;
    q->head = q->head->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    free(ptr_to_free);
    q->size--;
    return temp;
}

void q_push(queue* q, const int array) {
    queue_item* new_elem = malloc(sizeof(queue_item));
    new_elem->value = array;
    new_elem->next = 0;
    if (q->head == NULL) {
        q->head = new_elem;
        q->tail = new_elem;
        new_elem->prev = 0;
    } else {
        q->tail->next = new_elem;
        new_elem->prev = q->tail;
        q->tail = new_elem;
    }
    q->size++;
}

void q_print(const queue *q){
    if(q_empty(q)){
        printf("<queue is empty>\n");
        return;
    }
    printf("queue: ");
    queue_item *temp = q->head;
    while(temp != NULL){
        if(temp->next == NULL){
            printf("%d.", temp->value);
        }else{
            printf("%d, ", temp->value);
        }
        temp = temp->next;
    }
    printf("\n");
}


bool q_empty(const queue* q) {
    return q->head == NULL;
}

int q_size(const queue* q) {
    return q->size;
}

void q_destroy(queue* q) {
    queue_item* start = q->head;
    while (start != NULL) {
        queue_item* next = start->next;
        free(start);
        start = next;
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}