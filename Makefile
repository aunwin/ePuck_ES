# Compiler
CC = gcc

# Deklaration der Quelldateien
SRC = epuck.c
EXE = $(SRC:%.c=%)

# Flags für den C Compiler
CFLAGS = -I. -Wall -g -lreadline -lbluetooth

# Targetdefinition all
all: $(EXE)
	$(CC) -o $(EXE) $(SRC) $(CFLAGS)

# Targrgetdefinition clean
clean:
	rm -f $(OBJ) $(EXE)
	rm -rf html
	rm -rf latex
