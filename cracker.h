#ifndef CRACK_H
#define CRACK_H

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

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

// Define a cracing task and all attribus which related on this
typedef struct {
	uint16_t base;
	uint64_t keysize_max;
	char* charset;
	char* hash;
	char* salt;
	enum algo_num algorithm;

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

typedef struct {
	char* config_file;
	uint8_t deamon;
	crack_task* task;

} config_options;

enum algo_num {crypt = 0, md5};

unsigned long long int keyrange(crack_task);
void keynr_2_key(crack_task, int, char**);
int get_next_key(crack_task, char*, int);
int ben_next_key(crack_task, char*);
int compare_hash(char*, char*);
void usage(void);
void* start_crack_task(void*);
void init_crack_task(crack_task*);
int calculate_sub_task(crack_task*, crack_task*, int, int);

#endif
