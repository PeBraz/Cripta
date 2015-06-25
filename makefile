CC=gcc
XNAME=cripta
PASSWORD=1234
TEST_DIR=../test
TEST_CRIPTA=test_CRIPTA
VAL_FLAGS=--leak-check=full
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

test: all
	./${XNAME} -p ${PASSWORD} -c ${TEST_DIR}
	./${XNAME} -p ${PASSWORD} -d ${TEST_CRIPTA}

test-val: clean all
	- valgrind ${VAL_FLAGS} ./${XNAME} -p ${PASSWORD} -c ${TEST_DIR}
	- valgrind ${VAL_FLAGS} ./${XNAME} -p ${PASSWORD} -d ${TEST_CRIPTA}

clean:
	rm main.o list.o util.o cripta *.CRIPTA
