CC=gcc
CFLAGS=-I.
LIB = ../../target/debug/libcardano_c.a

main: main.c
	$(CC) -o main main.c $(LIB) -lpthread -lm -ldl -lcurl

transaction: transaction.c
	$(CC) -o transaction transaction.c $(LIB) -lpthread -lm -ldl

.PHONY: clean

clean:
	rm main transaction
