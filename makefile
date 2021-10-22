CC = gcc
CFLAGS = -Wall -ansi -pedantic -Werror -g
OBJ = lift_sim_A.o
EXEC = program

$(EXEC) : $(OBJ)
	$(CC) -pthread $(OBJ) -o $(EXEC)

lift_sim_A.o : lift_sim_A.c lift.h
	$(CC) -c lift_sim_A.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ)
