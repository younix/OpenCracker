#include <unistd.h>
#include <getopt.h>

/* Network */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "cracker.h"

int main(int argc, char **argv)
{
	crack_task task;
	int option = 0;
	int optionindex = 0;
	void* key; // returned key
	int status = 0;
	int thread_number = 1; // user defined number of threads
	int tnum = 0; // counter for for-loops
	thread_info *tinfo;

	// initialize task
	init_crack_task( &task);
	
	config_options config;

	parse_argument(argc, argv, config);

	if(config.config_file != NULL)
		read_config_file(config);
	
	// calculating and checking of user inputs
	task.keyrange = keyrange(task);
	printf("test\n\n");
	if(thread_number < 1)
		thread_number = 1;
	
	if(task.hash == NULL)
	{
		usage();
		return -1;
	}

	if(task.charset == NULL)
	{
		task.charset = (char*) calloc(sizeof(char), 4);
		strncpy(task.charset, "abc", 3);
	}

	// TODO: find a nice solution for this workaround
	div_t blub = div( task.keyrange, thread_number);
	task.keyarea_size = blub.quot;
	
	tinfo = calloc(thread_number, sizeof(thread_info));
	if(tinfo == NULL)
		printf("error: tinfo == NULL\n\n");;

	for(tnum = 0; tnum < thread_number; ++tnum)
	{
		tinfo[tnum].thread_num = tnum + 1;
		calculate_sub_task(&task, &tinfo[tnum].task, thread_number, tnum);

		status = pthread_create(&tinfo[tnum].thread_id, NULL, &start_crack_task, &tinfo[tnum].task);
	}

	for(tnum = 0; tnum < thread_number; ++tnum)
	{
		status = pthread_join(tinfo[tnum].thread_id, &key);

		if(key != NULL)
			printf("Find: \"%s\"\n\n", (char*) key);
		else
			printf("Not Found!\n\n");
	}
	
	if(key != NULL)
		free(key);
	
	if(task.hash != NULL)
		free(task.hash);
	
	if(task.charset != NULL)
		free(task.hash);
	
	if(tinfo != NULL)
		free(tinfo);

	return 0;
}

// parse the command line arguments
int argc_parser(int argc, char **argv, config_options *config)
{
	// define command line options
	struct option long_options[] =
	{
		{"help"		, 0, NULL, 'h'},
		{"hash"		, 1, NULL, 'a'},
		{"charset"	, 1, NULL, 'c'},
		{"length"	, 1, NULL, 'l'},
		{"file"		, 1, NULL, 'f'},
		{"threads"	, 1, NULL, 't'},
		{"server"	, 0, NULL, 's'},
		{"config"	, 1, NULL, ''},
		{0, 0, 0, 0}
	};

	// analysing command line options
	while((option = getopt_long( argc, argv, "ha:c:l:f:t:s::", long_options, &optionindex)) != -1)
	{
		switch (option)
		{
			case 'a':
				task.hash = (char*) calloc(sizeof(char), strlen(optarg) + 1);
				strncpy(task.hash, optarg, strlen(optarg));
				break;

			case 'c':
				task.base = strlen(optarg);
				task.charset = (char*) calloc(sizeof(char), task.base + 1);
				strncpy(task.charset, optarg, task.base);
				break;
			
			case 'l':
				for(int i = 0; i < strlen(optarg); ++i)
					if(isdigit(optarg[i]) == 0)
						return -1;

				task.keysize_max = atoi(optarg);
				break;

			case 'f':
				printf("File: %s\nThis option is not implemented!\n", optarg);
				break;

			case 't':
				for(int i = 0; i < strlen(optarg); ++i)
					if(isdigit(optarg[i]) == 0)
						return -1;

				thread_number = atoi(optarg);
				break;

			case 's':
				printf("This feature is not implemented!\n");
				break;

			case 'h':
			default:
				usage();
				break;
		}
	}

	return 0;
}

// TODO: open a listening socket and delegate the client requests
// TODO: the whole client-server communication have to be encrypted
int start_server(crack_task task, char* location)
{
	
	return 0;
}

// read configuration file and set the config
int read_config(config_option config, char *filename)
{
	FILE* fh = fopen(filename, "r");
	
	if(fh == NULL)
		return -1;

	fgets(fh);
	fclose(fh);

	return 0;
}

// print a message about the command line arguments
void usage(void)
{
	const char* usage_str =
	"usage: cracker <option>\n\n"
	"Option                   Discription\n\n"
	"-h, --hash <HASH>        Define a spezific hash to crack\n"
	"-c, --charset <CHARSET>  Define charset which the searched\n"
	"                         password consists of.\n"
	"-l, --length <NUM>       Maximum password length\n"
	"-f, --file <FILE>        Password file (like /etc/shadow)\n";

	printf("%s", usage_str);
}
