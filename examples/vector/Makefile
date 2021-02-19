NAME = vector
INPUT = $(NAME).c
OUTPUT = $(NAME)

CC = clang
OPT = -march=native -O2
VERSION = -std=c11

FLAGS = -Wall -Wextra -Werror -pedantic -pedantic-errors -Wpedantic
WARNINGS = -Wno-variadic-macros -Wno-declaration-after-statement -Wno-implicit-function-declaration -Wno-incompatible-pointer-types -Wno-int-conversion -Wno-string-compare
UNUSED_WARNINGS = -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-extra-semi
#COMPILER_WARNINGS = -ferror-limit=100
LIBS =


COMPILE = $(INPUT) $(NAME).spec.c

all: compiler

compiler:
	$(CC) $(OPT) $(VERSION) $(FLAGS) $(WARNINGS) $(UNUSED_WARNINGS) $(COMPILER_WARNINGS) $(LIBS) -o $(OUTPUT) $(COMPILE)
	./$(NAME)

clean:
	rm -f $(OUTPUT)
