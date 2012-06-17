# Compiler
CC = gcc

# Deklaration der Quelldateien
SRC = epuck.c
OBJ = $(SRC:%.c=%.o)
EXE = $(SRC:%.c=%)

# Flags für den C Compiler
CFLAGS = -I. -Wall -g

# Flags für den Linker
LDFLAGS = -lbluetooth -lreadline

# Targetdefinition all
all: $(EXE)

# Targetdefinition für Doxygen Dokumentation
docu:
	doxygen Doxyfile

# Targetdefinition clean
clean:
	rm -f $(OBJ) $(EXE)
	rm -rf html
	rm -rf latex
