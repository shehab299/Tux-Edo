build:
	gcc src/clk.c -o clk.out
	
	gcc -c src/Scheduler.c 
	gcc -c src/DataStructures/priorityQ.c
	gcc Scheduler.o priorityQ.o -o scheduler.out
	
	gcc src/Process.c -o process.out
	gcc tests/test_generator.c -o testgen.out

	gcc -c src/DataStructures/Queue.c
	gcc -c src/ProcessGen.c 

	gcc ProcessGen.o Queue.o -o pg.out

	rm -f *.o

	./pg.out /home/asmaa/Desktop/Tux-Edo

clean:
	rm -f *.o *.out

all: clean build run