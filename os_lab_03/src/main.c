#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <math.h>

pthread_mutex_t mutex;

typedef struct ThreadParams thread_params;
struct ThreadParams {
    int number_arrays; 
    int length;
    int number_length_threads; // тут еще не точно
    queue *q; 
    int move;  
};


typedef struct ThreadParams2 thread_params2;
struct ThreadParams2 {
	int *a;
	int *ressumm;
	int start;
	int move;
	int length;
};




void * asplitt(void *arg){
	thread_params2 targ2 = *(thread_params2 *) arg;
	int start = targ2.start;
	int move = targ2.move;
	int length = targ2.length;
	int finish = start + move;

	if(length < finish) finish = length;

	for(int i=start; i<finish; i++) targ2.ressumm[i] += targ2.a[i];
}



void * sumfunc(void * arg){ 
	thread_params targ = *(thread_params *) arg;
	int length = targ.length;
	int n = targ.number_arrays;
	int lt = targ.number_length_threads;
	int move = targ.move;
	int *a, *ressumm;

	ressumm = (int*)malloc(length * sizeof(int)); 

	
	if(lt){
	}



	for(int i=0; i<n; i++){
		if (!q_empty(targ.q)){
			pthread_mutex_lock(&mutex);
			a = q_pop(targ.q);
			pthread_mutex_unlock(&mutex);
			if(lt){
				
				pthread_t* threads2 = malloc(sizeof(pthread_t) * lt); //lt количество тредов
				thread_params2* params2 = malloc(sizeof(thread_params) * lt);
				for (int i = 0; i < lt; ++i) {
					params2[i].a = a;
					params2[i].ressumm = ressumm;
					params2[i].move = move;
					params2[i].start = i*move;
					params2[i].length = length;

						if( pthread_create(&threads2[i], NULL,
					 &asplitt, &params2[i]) != 0) {
						printf("Error create 2\n");
						exit(-1);
					}
				}

				for (int i = 0; i < lt; ++i) {
					pthread_join(threads2[i], NULL);
				}
			} else {
				for(int j=0; j<length; j++) ressumm[j] += a[j];	
			}
		} else break;
	}

	pthread_mutex_lock(&mutex);
	q_push(targ.q, ressumm);
	pthread_mutex_unlock(&mutex);
	return NULL;
}



int main(){
	int i, m, n, number_threads, number_length_threads, length, move;
	int *a;
	queue q;
	q_init(&q);
	printf("Enter the number and length of arrays\n");
	scanf("%d", &n); 
	scanf("%d", &length);

	printf("Enter number threads for addition arrays from 1 to %d\n", n/2);
	scanf("%d", &number_threads);
	m = n / number_threads; 

	if(length > 100){
		printf("Enter number threads for program from 1 to %d\n", length/25);
		scanf("%d", &number_length_threads);
		move = ceil(length / number_length_threads);
	} else number_length_threads = 0;

	
	pthread_t* threads = malloc(sizeof(pthread_t) * number_threads);
	thread_params params;
	params.q = &q;
	params.length = length; // длина массива
	params.number_arrays = m;
	params.number_length_threads = number_length_threads; // количество потоков для "разбиения" массива
	params.move = move; // по сколько элементов массива складываются в одном потоке


	for(i=0; i<n; i++){
		a = (int*)malloc(length*sizeof(int));
		for (int j = 0; j<length; j++) {
    	a[j] = 1;
  		}
  		q_push(&q,a);
	}


	while(q_size(&q) != 1){
		printf("I entered while\n");
		for(i=0; i<number_threads; ++i){
			if(pthread_create(&threads[i], NULL,
			 &sumfunc, &params) != 0){
				printf("Error create\n");
				return 1;
			}
		}
		for(i=0; i<number_threads; ++i){
			if(pthread_join(threads[i], NULL) != 0) {
				printf("Error join\n");
				return 1;
			}
		}
	}

	a = q_pop(&q);
	printf("Result array\n");
	for(i=0; i<length; i++) printf("%d ", a[i]);
	printf("\n");
	pthread_mutex_destroy(&mutex);
	q_destroy(&q);

	return 0;
}