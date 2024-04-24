build:
	gcc src/clk.c -o clk.out
	gcc -c src/Scheduler.c -o scheduler.out
	gcc src/Process.c -o process.out
	gcc tests/test_generator.c -o testgen.out
	gcc -c src/DataStructures/*.c
	gcc src/ProcessGen.c Queue.o src/DataStructures/*.o -o pg.out

clean:
	rm -f *.o *.out

all: clean build run

run:
	./pg.out	/home/khalid/Desktop/Project:Tux-Edo