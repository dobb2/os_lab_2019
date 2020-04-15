#include <unistd.h> // for fileno, read/write 
#include <stdlib.h> //for atoi/exit
#include <string.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/types.h>


void write_to_memory(int a, int* memory, int position){
  int* n = (int*)(memory + 4*position);
  *n = a;
}
int get_from_memory(int* memory, int position){
  int* res = (int*)(memory + 4*position);
  return *res;
}


int main(){  
  char num1[10]; 
  int num, summ;
  int i, m, n, length;
  int *ptr, *ptr2;
  int res1, res2;
  pid_t p;

  int fd = open("mapped.dat", O_RDWR | O_CREAT, 0666);
  if(fd == -1){
    printf("File open failed!\n");
    exit(1);
  }

  sem_t* semaphore = sem_open("lovesem", O_CREAT, 777, 0);
  sem_t* semaphore1 = sem_open("lovesem1", O_CREAT, 777, 0);
  if(semaphore == SEM_FAILED || semaphore1 == SEM_FAILED){
    printf("Semaphores doesn't create\n");
    exit(1);
  }
  sem_unlink("losesem");
  sem_unlink("lovesem1");

  printf("Enter m \n");
    read(STDIN_FILENO,num1,10);
    m = atoi(num1);
    if (!atoi(num1)){
      printf("m should not be zero \n");
      printf("enter number m\n");
      read(STDIN_FILENO,num1,10);
      m = atoi(num1);
    }

    printf("Enter n not equal to zero\n");
    read(STDIN_FILENO,num1,10);
    n = atoi(num1);

    length = m*n*sizeof(int) + 4*sizeof(int);
    ftruncate(fd,length);

    ptr = (int*)mmap(NULL, length, PROT_WRITE | 
      PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
      printf("Memmory mapping failed");
      exit(1);
    }

  p = fork();
  
  if(p == -1){ // everything is bad, the fork is not working
    printf("Can't fork child");
     exit(1);
  } else if(p > 0){
  	

    printf("Enter first matrix \n");
    for(i=2;i<(m*n+2);i++) {
      scanf("%d", &num);
      write_to_memory(num,ptr,i);
    }
      
    printf("Enter second matrix \n");     
    for(i=(m*n+2);i<(2*m*n+2);i++) { 
      scanf("%d", &num); 
      write_to_memory(num,ptr,i); 
    }
  
    sem_post(semaphore);

    sem_wait(semaphore1);//тут ждем
       
    printf("Means value first matrix\n");
    res1 = get_from_memory(ptr,0);
    printf("%d\n", res1);

    printf("Means value second matrix\n");
    res2 = get_from_memory(ptr,1);
    printf("%d\n", res2);

    sem_close(semaphore);
    sem_close(semaphore1);
    munmap(ptr,length);
    close(fd);
  
  } else if (p == 0){  
  	int res1, res2; 

    sem_wait(semaphore);

   
    for(i = 2;i < (m*n+2);i++){
      num = get_from_memory(ptr,i);
      res1+=num;
    }
    res1 = res1 / (m*n);

    for(i = (m*n+2);i < (2*m*n+2);i++){
      num = get_from_memory(ptr,i);
      res2+=num;
    }
    res2 = res2 / (m*n);

    write_to_memory(res1,ptr,0);
    write_to_memory(res2,ptr,1);

    sem_post(semaphore1);

    sem_close(semaphore);
    sem_close(semaphore1);
    close(fd);
  }  
  return 0;
}