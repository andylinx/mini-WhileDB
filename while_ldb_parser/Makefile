lexer.h: lang.l
	flex lang.l

lexer.c: lang.l
	flex lang.l

parser.c: lang.y
	bison -o parser.c -d -v -Wcounterexample lang.y

parser.h: lang.y
	bison -o parser.c -d -v lang.y

lang.o: lang.c lang.h
	gcc -c lang.c

lib.o: lib.c lib.h lang.h
	gcc -c lib.c

parser.o: parser.c parser.h lexer.h lang.h
	gcc -c parser.c

lexer.o: lexer.c lexer.h parser.h lang.h
	gcc -c lexer.c

interpreter.o: interpreter.c lang.h lib.h
	gcc -c interpreter.c

main.o: main.c lexer.h parser.h lang.h
	gcc -c main.c

main: lang.o parser.o lexer.o interpreter.o lib.o main.o
	gcc lang.o parser.o lexer.o interpreter.o lib.o main.o -o main

all: main

clean:
	rm -f lexer.h lexer.c parser.h parser.c *.o main

%.c: %.y

%.c: %.l

.DEFAULT_GOAL := all

