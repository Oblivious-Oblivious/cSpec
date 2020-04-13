INCL = vector.h
SRC = vector.c vector.spec.c
OBJ = $(SRC:.c=.o)
LIBS =
EXE = vector

CC = /usr/bin/gcc
CFLAGS = -O3 -Wno-int-conversion -Wno-incompatible-pointer-types -Wno-pointer-to-int-cast
LIBPATH = -L.
LDFLAGS = -o $(EXE) $(LIBPATH) $(LIBS)
CFDEBUG = -Wall -g -DDEBUG $(LDFLAGS)
RM = /bin/rm -f

%.o: %.c
	$(CC) -c $(CFLAGS) $*.c

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ)

$(OBJ): $(INCL)

debug:
	$(CC) $(CFDEBUG) $(SRC)

clean:
	$(RM) $(OBJ) $(EXE) output.txt