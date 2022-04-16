all: 
	mpicc foxes-rabbits.c -Wall -o foxes-rabbits -O3 -g -fopenmp

clean:
	@echo Cleaning...
	rm -f *.o foxes-rabbits