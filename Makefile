FLAGS = -Wall -O3 -g -fopenmp

all: common.o worldManager.o entityMovement.o simulationRunner.o foxes-rabbits.o
	mpicc $(FLAGS) -o foxes-rabbits foxes-rabbits.o simulationRunner.o entityMovement.o worldManager.o common.o

common.o: common.h
	mpicc $(FLAGS) -o common.o -c common.c

worldManager.o: common.h worldManager.c
	mpicc $(FLAGS) -o worldManager.o -c worldManager.c

entityMovement.o: common.h worldManager.h entityMovement.c
	mpicc $(FLAGS) -o entityMovement.o -c entityMovement.c

simulationRunner.o: common.h worldManager.h entityMovement.h simulationRunner.c
	mpicc $(FLAGS) -o simulationRunner.o -c simulationRunner.c

foxes-rabbits.o: common.h worldManager.h entityMovement.h simulationRunner.h foxes-rabbits.c
	mpicc $(FLAGS) -o foxes-rabbits.o -c foxes-rabbits.c

clean:
	@echo Cleaning...
	rm -f *.o foxes-rabbits