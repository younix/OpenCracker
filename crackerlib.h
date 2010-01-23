#ifndef CRACKLIB_H
#define CRACKLIB_H

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

//#ifdef LINUX
#include <crypt.h>
//#endif

#ifdef __OBSD__
#include <pwd.h>
#include <unistd.h>
#endif

#include <pthread.h>
#include <ctype.h>

/* algorithm */
#include "algo/crypt.h"

enum algo_num {gnu_crypt = 0, md5, md4, md2, sha512};

// Define a cracking task and all attribute which related on this
typedef struct {
	uint16_t base;			// the number of characters in charset
	uint64_t keysize_max;		// the maximum length of a key
	char* charset;			// includes all character which is used in a key
	char* hash;			// the hash which is gone to crack
	char* salt;			// if a salt is used, this should be defined here
	enum algo_num algorithm;	// used algorithm for the hash

	//additional information
	uint64_t keyrange;

	// for splitting calculation, e.g. threading or distributed computing
	char* start_key;
	uint64_t keyarea_size;
} crack_task;

typedef struct {
	pthread_t thread_id;
	uint8_t thread_num;
	crack_task task;
} thread_info;


unsigned long long int keyrange(crack_task);
void keynr_2_key(crack_task, int, char**);
int get_next_key(crack_task, char*, int);
int ben_next_key(crack_task, char*);
bool compare_hash(char*, char*, enum algo_num);
void* start_crack_task(void*);
void init_crack_task(crack_task*);
void free_crack_task(crack_task*);
int calculate_sub_task(crack_task*, crack_task*, int, int);

#endif
