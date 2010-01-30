cracker: cracker.c cracker.h crackerlib.c crackerlib.h
	gcc -g -std=c99 -pedantic -Wall -o cracker cracker.c crackerlib.c -lm -lcrypt -pthread
obsd: cracker.c cracker.h
	gcc -std=c99 -Wall -o cracker cracker.c algo/crypt.c -lm -pthread
benchmark: benchmark.c
	gcc -Wall -std=c99 -pedantic -o bench benchmark.c -lgnutls-openssl
