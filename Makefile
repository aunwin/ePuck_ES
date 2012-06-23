all: $(EXE)
	gcc -o epuck epuck.c -I. -Wall -g -lreadline -lbluetooth -L/usr/lib/x86_64-linux-gnu -lSDL -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT

# Targrgetdefinition clean
clean:
	rm -f $(OBJ) $(EXE)
	rm -rf html
	rm -rf latex
