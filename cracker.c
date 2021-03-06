#include "cracker.h"

int main(int argc, char **argv)
{
	//-- initialisation --//
	crack_task task;
	int status = 0;
	char* key;
	thread_info *tinfo;

	// initialize task
	init_crack_task(&task);
	
	config_options config;	// hold the configuration options 
	parse_argument(argc, argv, &config, &task);	// parse user arguments from command line

	if(read_config_file(&config))
		printf("unable to read config file.\n");
	
	// calculating and checking of user inputs
	task.keyrange = keyrange(task);

	// TODO: find a nice solution for this workaround
	div_t blub = div( task.keyrange, config.thread_number);
	task.keyarea_size = blub.quot;
	
	tinfo = calloc(config.thread_number, sizeof(thread_info));
	if(tinfo == NULL)
		printf("error: tinfo == NULL\n\n");;

	for(int tnum = 0; tnum < config.thread_number; ++tnum)
	{
		tinfo[tnum].thread_num = tnum + 1;
		calculate_sub_task(&task, &tinfo[tnum].task, config.thread_number, tnum);

		status = pthread_create(&tinfo[tnum].thread_id, NULL, &start_crack_task, &tinfo[tnum].task);
	}

	for(int tnum = 0; tnum < config.thread_number; ++tnum)
	{
		status = pthread_join(tinfo[tnum].thread_id, (void**) &key);

		if(key != NULL)
			printf("Find: \"%s\"\n\n", (char*) key);
		else
			printf("Not Found!\n\n");
	}
	
	if(key != NULL)
		free(key);
	
	if(tinfo != NULL)
		free(tinfo);

	free_crack_task(&task);

	return 0;
}

// parse the command line arguments and save them in struct config
void parse_argument(int argc, char **argv, config_options *config, crack_task *task)
{
	int option = 0;
	int optionindex = 0;
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
		{"config"	, 1, NULL, 'C'},
		{0, 0, 0, 0}
	};

	// analysing command line options
	while((option = getopt_long( argc, argv, "ha:c:l:f:t:s::", long_options, &optionindex)) != -1)
	{
		switch (option)
		{
			case 'a':
				task->hash = (char*) calloc(sizeof(char), strlen(optarg) + 1);
				strncpy(task->hash, optarg, strlen(optarg));
				break;

			case 'c':
				task->base = strlen(optarg);
				task->charset = (char*) calloc(sizeof(char), task->base + 1);
				strncpy(task->charset, optarg, task->base);
				break;
			
			case 'l':
				for(int i = 0; i < strlen(optarg); ++i)
					if(isdigit(optarg[i]) == 0)
						exit(-1);

				task->keysize_max = atoi(optarg);
				break;

			case 'f':
				printf("File: %s\nThis option is not implemented!\n", optarg);
				break;

			case 't':
				for(int i = 0; i < strlen(optarg); ++i)
					if(isdigit(optarg[i]) == 0)
						exit(-1);

				config->thread_number = atoi(optarg);
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
}

// TODO: open a listening socket and delegate the client requests
// TODO: the whole client-server communication have to be encrypted
int start_server(crack_task task, char* location)
{
	
	return 0;
}

// read configuration file and save optons to config
bool read_config_file(config_options *config)
{
	char* option;
	char* value;
	FILE* fh;
	
	option = (char*) calloc(100, sizeof(char));
	value  = (char*) calloc(100, sizeof(char));

	if(config->config_file == NULL)
		fh = fopen(CONFIG_FILE, "r");
	else
		fh = fopen(config->config_file, "r");

	if(fh == NULL)
		return false;
	
	char *line;
	line = (char*) calloc(100, sizeof(char));

	while (!feof(fh)) {
		
		//fgets(line, 99, fh);

		//this is an unsave way of doing this
		//TODO: find a better way
		fscanf(fh, "%s = %s", option, value);

		set_config_option(config, option, value);
	}


	//fgets(fh);
	fclose(fh);

	return true;
}

bool set_config_option(config_options *config, char *option, char *value)
{
	if( strcmp(option, "threads") && isdigit(option) )
	{
		config->thread_nr = atoi(value);
		return true;
	}

	return false;
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
