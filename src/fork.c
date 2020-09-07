#include <sys/types.h>
#include <unistd.h> // for fileno, read/write 
#include <stdlib.h> //for atoi/exit
#include <string.h>


void ftoa(float n, char s[]) //float_to_char
 {
    int i, sign, k, d;
 
    if ((sign = n) < 0)  /* записываем знак */
        n = -n;          /* делаем n положительным числом */
    i = 0;
    k = n;
    n = n - k;
    while (n - (int)n)
        n*=10;
    d = (int)n;
    do {       /* генерируем цифры после точки в обратном порядке */
        s[i++] = d % 10 + '0';   /* берем следующую цифру */
    } while ((d /= 10) > 0);     /* удаляем */
    s[i++] = '.';
    do {
        s[i++] = k % 10 + '0';
    } while ((k /= 10) > 0);
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }



int main(){   
    char num1[10], num2[10], s; 
    int trub1[2]; 
    int trub2[2];
    double x=0, summ;
    int i, m, n;
    pid_t p;

    if(pipe(trub1)<0) return 1;
    if(pipe(trub2)<0) return 1;
 
    p = fork(); // create two process

    if(p == -1){ // everything is bad, the fork is not working
      return -1;
    } else if (p == 0){

      read(trub1[0], num2, sizeof(num2));
      m = atoi(num2);
      read(trub1[0], num2, sizeof(num2));
      n = atof(num2);

      // for first matrix
      for(i=0;i<m*n;i++){
        read(trub1[0], num2, sizeof(num2));
        x +=atof(num2);
      }

      summ = x / (m*n); // means value first matrix
      x = 0;
      ftoa(summ,num2);
      write(trub2[1], &num2, sizeof(num2));

      //for second matrix
      for(i=0;i<m*n;i++){
        read(trub1[0], num2, sizeof(num2));
        x +=atof(num2);
      }
      summ = x / (m*n);
      ftoa(summ,num2);
      write(trub2[1], &num2, sizeof(num2));
      
   //child process
    } else {
  
      read(STDIN_FILENO,num1,10);
      m = atoi(num1);
      if (!atoi(num1)){
        write(STDOUT_FILENO, "m should not be zero \n", sizeof "m should not be zero\n" - 1);
        write(STDOUT_FILENO, "enter number m\n", sizeof "enter number m\n" - 1);
        read(STDIN_FILENO,num1,10);
        m = atoi(num1);
      }
      write(trub1[1], &num1, sizeof(num1));
      write(STDOUT_FILENO, "enter n not equal to zero\n", sizeof "enter n not equal to zero\n" - 1);
      read(STDIN_FILENO,num1,10);
      n = atoi(num1);
      write(trub1[1], &num1, sizeof(num1));


      for(i=0;i<m*n;i++) {
        read(STDIN_FILENO,num1,10);
        write(trub1[1], &num1, sizeof(num1));
      }
     
            
      for(i=0;i<m*n;i++) {
        read(STDIN_FILENO,num1,15);
        write(trub1[1], &num1, sizeof(num1));
      }
      
              
      read(trub2[0], &num1, sizeof(num1));
      write(STDOUT_FILENO, "Means value first matrix\n", sizeof "Means value first matrix\n" - 1);
      write(STDOUT_FILENO,&num1,strlen(num1));
      s = '\n';
      write(STDOUT_FILENO,&s, sizeof(s));
      
      read(trub2[0], &num1, sizeof(num1)); 
      write(STDOUT_FILENO, "Means value second matrix\n", sizeof "Means value second matrix\n" - 1);
      write(STDOUT_FILENO,&num1,strlen(num1));
      write(STDOUT_FILENO,&s, sizeof(s));
      //parent process 
    }   
    return 0;
}