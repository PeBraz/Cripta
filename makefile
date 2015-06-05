CC=gcc
XNAME=cripta
all: list.o main.o
	${CC} -g -o ${XNAME} list.o main.o -lssl -lcrypto

main.o:
	${CC} -c main.c
list.o:
	${CC} -c list.c

val: all
	valgrind --leak-check=full ./${XNAME} .
clean:
	rm main.o list.o cripta
