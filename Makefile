all: Manager/manager.o main.o
	gcc -Wall -o foxes-rabbits Manager/manager.o main.o

Manager/manager.o: Manager/worldManager.c Manager/worldManager.h
	gcc -Wall -o Manager/manager.o -c Manager/worldManager.c 

main.o: main.c Manager/worldManager.h
	gcc -Wall -o main.o -c main.c

clean:
	@echo Cleaning...
	rm -f *.o foxes-rabbits