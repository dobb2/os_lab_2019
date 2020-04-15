#include "queue.h"
#include <dlfcn.h>
#include <stdio.h>

/*
typedef struct QueueItem queue_item;
struct QueueItem;

struct Queue;
typedef struct Queue queue;
*/


void menu() {
	printf("\nMenu -- 1\n");
	printf("Push -- 2\n");
	printf("Pop -- 3\n");
	printf("Show top element -- 4\n");
	printf("Print queue -- 5\n");
	printf("Print size queue -- 6\n");
	printf("Exit -- 7\n");
	printf("Empty or nor -- 8\n");
}

int main(){
	int n, digit;
	bool t = true;
	void *ext_library;



	menu();

	ext_library = dlopen("libqueue.so", RTLD_LAZY);
	if (!ext_library){
		//если ошибка, то вывести ее на экран
		fprintf(stderr,"dlopen() error: %s\n", dlerror());
		return 1;
	};

	//queue_item (*QueueItem)(struct QueueItem* next, struct QueueItem* prev, int *value) = dlsym(ext_library, "QueueItem");
	//queue (*Queue)(queue_item* head, queue_item* tail, int *size) = dlsym(ext_library, "Queue");
	void (*q_init)(queue* q) = dlsym(ext_library,"q_init");
	int (*q_top)(const queue* q) = dlsym(ext_library, "q_top");
	int (*q_pop)(queue* q) = dlsym(ext_library, "q_pop");
	int (*q_size)(const queue* q) = dlsym(ext_library, "q_size");
	void (*q_push)(queue* q, const int arrray) = dlsym(ext_library, "q_push");
	bool (*q_empty)(const queue* q) = dlsym(ext_library, "q_empty");
	void (*q_destroy)(queue* q) = dlsym(ext_library, "q_destroy");
	void (*q_print)(const queue* q) = dlsym(ext_library, "q_print");

	queue q;
	(*q_init)(&q);
	
	while(t){
		printf("Enter the number to select an operation from the menu\n");
		scanf("%d", &n);
		while(n == 0){
			printf("Invalid input of a number.\n"); 
			printf("Enter a number from 1 to 7 to select an operation from the menu\n");
			scanf("%d", &n);
		}
		switch(n){
			case 1:
				menu();
				break;
			case 2:
				printf("Enter a digit:\n");
				scanf("%d", &digit);
				(*q_push)(&q, digit);
				break;
			case 3:
				(*q_pop)(&q);
				break;
			case 4:
				printf("The head of element: ");
				printf("%d\n", (*q_top)(&q));
				printf("\n");
				break;
			case 5:
				(*q_print)(&q);
				break;
			case 6:
				printf("Size queue: ");
				printf("%d\n", (*q_size)(&q));
				break;
			case 7:
				t = false;
				break;
			case 8:
				if( (*q_empty)(&q) )
					printf("Queue is empty\n");
				else
					printf("Queue is not empty");
				break;
		}
	}
	(*q_destroy)(&q);
	dlclose(ext_library);
	return 0;
}