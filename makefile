WARNS = -W -Wall -pedantic -Wno-comment -Wno-variadic-macros -Wno-unused-function

all:
	gcc test.c hash.c -std=c89 -Wall -pedantic -O3 -o hash

test:
	./hash

clean:
	rm -rf hash