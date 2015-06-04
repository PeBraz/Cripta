all: list.o main.o
	gcc -o cripta list.o main.o -lssl -lcrypto

main.o:
	gcc -c main.c
list.o:
	gcc -c list.c

clean:
	rm main.o list.o cripta