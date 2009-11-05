cracker: cracker.c cracker.h
	gcc -std=c99 -pedantic -Wall -o cracker cracker.c -lm -lcrypt -pthread
obsd: cracker.c cracker.h
	gcc -std=c99 -Wall -o cracker cracker.c -lm -pthread
benchmark: benchmark.c
	gcc -Wall -std=c99 -pedantic -o bench benchmark.c -lgnutls-openssl
