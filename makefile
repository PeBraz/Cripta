CC=gcc
XNAME=cripta
all: list.o main.o util.o
	${CC} -o ${XNAME} list.o main.o util.o -lssl -lcrypto

main.o:
	${CC} -g -c main.c
list.o:
	${CC} -g -c list.c
util.o:
	${CC} -g -c util.c

val: all
	valgrind --leak-check=full ./${XNAME} .
clean:
	rm main.o list.o util.o cripta
