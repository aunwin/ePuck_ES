all: $(EXE)
	gcc -o epuck epuck.c -I. -Wall -g -lreadline -lbluetooth

# Targrgetdefinition clean
clean:
	rm -f $(OBJ) $(EXE)
	rm -rf html
	rm -rf latex
