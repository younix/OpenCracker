#include <unistd.h>
#include <getopt.h>

/* Netzwork */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "cracker.h"

int main( int argc, char **argv)
{
	crack_task task;
	int option = 0;
	int optionindex = 0;
	void* key; // returned key
	int status = 0;
	int thread_number = 1; // user defined number of threads
	int tnum = 0; // counter for for-loops
	thread_info *tinfo;

	// initilize crack struct
	init_crack_task( &task);

	// define commandline options
	struct option long_options[] =
	{
		{"help"		, 0, NULL, 'h'},
		{"hash"		, 1, NULL, 'a'},
		{"charset"	, 1, NULL, 'c'},
		{"length"	, 1, NULL, 'l'},
		{"file"		, 1, NULL, 'f'},
		{"threads"	, 1, NULL, 't'},
		{"server"	, 0, NULL, 's'},
		{0, 0, 0, 0}
	};

	// analysing commandline options
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
				printf("This feature is noch implemented!\n");
				break;

			case 'h':
			default:
				usage();
				break;
		}
	}
	
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

	// ToDo: find a nice solution for this workaround
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

	return 0;
}

int start_server(crack_task task, char* location)
{
	
	return 0;
}

int calculate_sub_task(crack_task* task, crack_task* subtask, int thread_number, int tnum)
{
	int keynr_beginn = 0;

	init_crack_task(subtask);

	if(task->charset 	!= NULL) subtask->charset 	= (char*) calloc(strlen(task->charset) 	+ 1, sizeof(char));
	if(task->hash 		!= NULL) subtask->hash 		= (char*) calloc(strlen(task->hash)  	+ 1, sizeof(char));
	if(task->salt   	!= NULL) subtask->salt 		= (char*) calloc(strlen(task->salt) 	+ 1, sizeof(char));

	if(task->charset 	!= NULL) strncpy(subtask->charset, task->charset, task->base);
	if(task->hash		!= NULL) strncpy(subtask->hash, task->hash, strlen(task->hash));
	if(task->salt		!= NULL) strncpy(subtask->salt, task->salt, strlen(task->salt));

	subtask->base 		= task->base;
	subtask->keysize_max 	= task->keysize_max;
	subtask->algorithm 	= task->algorithm;
	subtask->keyrange	= task->keyrange;
	subtask->keyarea_size	= task->keyarea_size;

	keynr_beginn = task->keyarea_size * tnum;
	keynr_2_key(*task, keynr_beginn, &subtask->start_key);

	return 0;
}

// convert a position of a key in the keyrange to the spezific key
void keynr_2_key(crack_task crack, int key_nr, char **key)
{
	int char_nr = 0;
	int i = 0;
	int keylen = 0;

	if(*key == NULL)
		*key = (char*) calloc(crack.keysize_max + 1, sizeof(char));
	
	if(key_nr == 0)
	{
		*key[0] = '\0';
		return;
	}
	
	for(keylen = 0; key_nr >= pow(crack.base, keylen); ++keylen)
		key_nr -= pow(crack.base, keylen);
	
	do
	{
		char_nr = key_nr % crack.base;
		key_nr = key_nr / crack.base;
		(*key)[i] = crack.charset[char_nr];
		i++;
	}
	while(key_nr != 0 || strlen(*key) != keylen);
}

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

// single cracking thread
void* start_crack_task(void* arg)
{
	char* key;
	crack_task* task = (crack_task *) arg;

	int counter = 0;
	key = (char*) calloc(sizeof(char), task->keysize_max + 1);
	
	if(task->start_key != NULL)
		strncpy(key, task->start_key, task->keysize_max);
	
	do
	{
		if(compare_hash(key, task->hash) == 1)
		{
			printf("Thread found Key: %s\n hash:%s\n\n", key, task->hash);
			return key;
		}
		++counter;
	}
	while(get_next_key(*task, key, 0) == 0 && counter <= task->keyarea_size);
	//while(ben_next_key(crack, key) == 0); // second implementation

	free(key);

	return NULL; 
}

//calculate the keyrange
unsigned long long int keyrange(crack_task crack)
{
	int i;
	long long int keyrange = 0;
	for(i = 0; i <= crack.keysize_max; ++i)
		keyrange += pow(crack.base, i);
	
	return keyrange;
}

//increas the key by one
int get_next_key(crack_task crack, char* key, int pos)
{
	int i = 0;
	
	for(i = 0; i < crack.base; ++i)
	{
		if(key[pos] == crack.charset[i])
		{
			if((i + 1) < crack.base)
			{
				key[pos] = crack.charset[i+1];
				return 0;
			}
			else
			{
				key[pos] = crack.charset[0];
				if((pos + 1) < crack.keysize_max)
					return get_next_key(crack, key, pos + 1);
				else
					return -1;;
			}
		}
	}

	key[pos] = crack.charset[0];

	return 0;
}

//ben's version of next string
int ben_next_key(crack_task crack, char *key) {
	int i;
	int l = strlen(key);
	for (i = 0; i < l; ++i) { 
		if (key[i] == crack.charset[crack.base-1]) {
			if (i >= crack.keysize_max-1)
				return -1;
			key[i] = crack.charset[0];
			if (i == l-1) {
				key[i+1] = crack.charset[0];
				// should be ensured by calloc, but safety first!
				key[i+2] = '\0';
				// not needed (for loop would end NOW also without this) - but for readability of the algorithm
				break;
			}
		} else {
			key[i] = *(strchr(crack.charset, key[i])+1);
			break;
		}
	}
	// just a little feature, if a empty key is given
	// the first character in the base is used as first key
	if (l == 0)
		key[0] = crack.charset[0];
	return 0;
}

// set a task to zero
void init_crack_task(crack_task* task)
{
	task->base = 0;
	task->keysize_max = 0;
	task->charset = NULL;
	task->hash = NULL;
	task->salt = NULL;
	task->algorithm = 0;
	task->keyrange = 0;
	task->start_key = NULL;
	task->keyarea_size = 0;
}

//compare
int compare_hash(char* key, char* hash, enum algo_num algo)
{
	char* key_hash;
	
	switch(algo)
	{
		case crypt: key_hash = (char*) crypt(key, hash); break;
	}

	if(strncmp(key_hash, hash, strlen(key_hash)) == 0)
	{
		//printf("Key found: \"%s\" hash: \"%s\"\n\n", key, key_hash);
		return 1;
	}
	
	return 0;
}
