NAME = simple

CC = clang
OPT = -O2
VERSION = -std=c99

FLAGS =
WARNINGS = -Wall -Wextra -Werror -pedantic -pedantic-errors -Wpedantic
UNUSED_WARNINGS = -Wno-unused-function
LIBS =

INPUT = $(NAME).c
OUTPUT = $(NAME)
COMPILE = $(INPUT) $(NAME).spec.c

all: compiler

compiler:
	$(CC) $(OPT) $(VERSION) $(FLAGS) $(WARNINGS) $(UNUSED_WARNINGS) $(LIBS) -o $(OUTPUT) $(COMPILE)
	./$(NAME)

clean:
	$(RM) -r $(OUTPUT) ./*.dSYM ./*.txt ./*.xml ./*.md ./*.html
