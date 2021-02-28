all: default

default:
	cd src && make

lib: default

test:
	@echo

spec: test

clean:
	$(RM) -r export
