all: default

default:
	./compile

lib: default

test:
	@echo

spec: test

clean:
	$(RM) -r export
