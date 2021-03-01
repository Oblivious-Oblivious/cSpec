all: default

default:
	#sed -i -e 's/\r$//' compile
	./compile

lib: default

test:
	@echo

spec: test

clean:
	$(RM) -r export
