#include "queue.h"


void menu() {
	printf("\nMenu -- 1\n");
	printf("Push -- 2\n");
	printf("Pop -- 3\n");
	printf("Show top element -- 4\n");
	printf("Print queue -- 5\n");
	printf("Print size queue -- 6\n");
	printf("Exit -- 7\n");
}

int main(){
	queue q;//////????
	q_init(&q);
	int n, digit, top;
	bool t = true;

	menu();
	
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
				q_push(&q, digit);
				break;
			case 3:
				q_pop(&q);
				break;
			case 4:
				printf("The head of element: ");
				printf("%d\n", q_top(&q));
				printf("\n");
				break;
			case 5:
				q_print(&q);
				break;
			case 6:
				printf("Size queue: ");
				printf("%d\n", q_size(&q));
				break;
			case 7:
				t = false;
				break;
		}
	}
	q_destroy(&q);
	return 0;
}