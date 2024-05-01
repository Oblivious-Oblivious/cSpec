NAME = simple

CC = clang
OPT = -O2
VERSION = -std=c89

FLAGS = -Wall -Wextra -Werror -pedantic -pedantic-errors -Wpedantic
WARNINGS = -Wno-variadic-macros
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
	rm -f $(OUTPUT) ./*.txt ./*.xml ./*.md ./*.html
