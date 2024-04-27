build:
	gcc src/clk.c -o clk.out
	
	gcc -c src/Scheduler.c 
	gcc -c src/DataStructures/PriorityQueue.c
	gcc -c src/DataStructures/Queue.c
	gcc Scheduler.o PriorityQueue.o  Queue.o  -o scheduler.out
	
	gcc src/Process.c -o process.out
	gcc tests/test_generator.c -o testgen.out


	gcc -c src/ProcessGen.c 
	gcc ProcessGen.o Queue.o -o pg.out

	rm -f *.o

	./pg.out /home/asmaa/Desktop/Tux-Edo

test_c:
	gcc -c tests/test_cq.c
	gcc -c src/DataStructures/CircularQueue.c

	gcc test_cq.o CircularQueue.o -o test.o
	./test.o

test_p:
	gcc -c tests/test_pq.c
	gcc -c src/DataStructures/PriorityQueue.c

	gcc test_pq.o PriorityQueue.o -o test.o
	./test.o

clean:
	rm -f *.o *.out

all: clean build run