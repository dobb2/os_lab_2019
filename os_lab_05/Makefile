all: main main_dyn

main: libqueue.so main.c
	gcc main.c -L. -lqueue -o main
	echo main is builded

main_dyn: libqueue.so main_dyn.c
	gcc -Wall -Werror -Wextra -ldl main_dyn.c -o main_dyn
	echo main_dyn is builded

libqueue.so: queue.o
	gcc -shared queue.o -o libqueue.so
	echo libqueue.so is builded

queue.o: queue.c
	gcc -Wall -Werror -Wextra -fPIC -c queue.c

clean:
	rm -rf *.so
	rm -rf *.o