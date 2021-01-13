NAME = simple

CC = /usr/bin/clang
OPT = -march=native -O2
#VERSION = -std=c89
OUTPUT = a.out

FLAGS = -Wall -Wextra -Werror -pedantic -pedantic-errors -Wpedantic
PRINT_WARNINGS = -Wno-int-conversion -Wno-format -Wno-unused-parameter -Wno-unused-function
WARNINGS = $(PRINT_WARNINGS) -Wno-void-pointer-to-int-cast -Wno-int-conversion -Wno-incompatible-pointer-types -Wno-sign-compare -Wno-string-compare -Wno-extra-semi
HEADERS =
LIBS =

COMPILE = $(NAME).c $(NAME).spec.c

all: compiler

compiler:
	$(CC) $(OPT) $(VERSION) $(HEADERS) $(FLAGS) $(WARNINGS) $(LIBS) -o $(OUTPUT) $(COMPILE)
	./$(OUTPUT)

clean:
	rm -rf $(OUTPUT)
