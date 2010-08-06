CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -Werror -O2
LIBS= -lm

keygen: keygen.c
	$(CC) $(CFLAGS) $(LIBS) keygen.c -o keygen
cracker: cracker.c cracker.h crackerlib.c crackerlib.h
	$(CC) $(CFLAGS) $(LIBS) -o cracker cracker.c crackerlib.c
debug: cracker.c cracker.h crackerlib.c crackerlib.h
	$(CC) $(CFLAGS) $(LIBS) -o cracker cracker.c crackerlib.c -g
obsd: cracker.c cracker.h
	$(CC) $(CFLAGS) -o cracker cracker.c algo/crypt.c -lm -pthread
benchmark: benchmark.c
	$(CC) $(CFLAGS) -o bench benchmark.c -lgnutls-openssl
