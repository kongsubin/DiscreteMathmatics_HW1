CC = gcc 
TARGETS = puzzle

$(TARGETS) : 

        $(CC) puzzle.c -o puzzle 

clean : 
        rm puzzle
