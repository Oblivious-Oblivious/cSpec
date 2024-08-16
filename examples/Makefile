NAME = examples

CC = clang
OPT = -Og
VERSION = -std=c99

FLAGS = -fsanitize=address -g
WARNINGS = -Wall -Wextra -Werror -pedantic -pedantic-errors -Wpedantic
UNUSED_WARNINGS =
LIBS =

INPUT = ./*.c
OUTPUT = $(NAME)
COMPILE = $(INPUT)

all: compiler

compiler:
	$(CC) $(OPT) $(VERSION) $(FLAGS) $(WARNINGS) $(UNUSED_WARNINGS) $(LIBS) -o $(OUTPUT) $(COMPILE)
	./$(NAME)

clean:
	$(RM) -r $(OUTPUT) ./*.dSYM
