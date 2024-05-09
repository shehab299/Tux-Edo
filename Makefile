run: build
	./pg.out /home/asmaa/Desktop/Tux-Edo

build: clean
	gcc src/clk.c -o clk.out
	gcc src/Process.c -o process.out

	gcc -c src/DataStructures/PriorityQueue.c
	gcc -c src/DataStructures/Queue.c
	gcc -c src/BuddySystem.c

	gcc -c src/Scheduler.c 
	gcc Scheduler.o PriorityQueue.o  Queue.o BuddySystem.o -lm -o  scheduler.out
	
	gcc -c src/ProcessGen.c 
	gcc ProcessGen.o Queue.o -o pg.out

clean:
	rm -f *.o *.out *.log *.perf

test:
	gcc tests/test_generator.c -o test.out
	./test.out