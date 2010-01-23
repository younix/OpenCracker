#ifndef CRACK_H
#define CRACK_H

#include <unistd.h>
#include <getopt.h>

/* Network */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

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

/* crackerlib */
#include "crackerlib.h"

/* algorithm */
#include "algo/crypt.h"

typedef struct {
	char* config_file;
	uint8_t deamon;
	uint8_t thread_number;
	char* key;
	//server_addr;
	crack_task* task;

} config_options;

void usage(void);
void parse_argument(int argc, char **argv, config_options* config, crack_task* task);
int read_config_file(config_options *config, char *filename);

#endif
