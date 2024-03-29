## CC  = Compiler.
## CFLAGS = Compiler flags.
CC	= gcc
CFLAGS =	-Wall -Wextra -std=gnu99


## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

SRC =		main.c list.c simulation.c computer.c algorithm.c
OBJ =		main.o list.o simulation.o computer.o algorithm.o
EXE = 		swap

## Top level target is executable.
$(EXE):	$(OBJ)
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ) -lm


## Clean: Remove object files and core dump files.
clean:
		/bin/rm $(OBJ)

## Clobber: Performs Clean and removes executable file.

clobber: clean
		/bin/rm $(EXE)

## Dependencies

main.o:			simulation.h
list.o:			list.h
simulation.o:	simulation.h computer.h list.h
computer.o:		computer.h simulation.h algorithm.h list.h
algorithm.o: 	algorithm.h computer.h simulation.h list.h
