#include <unistd.h>

#include "crackerlib.h"

// divide the part of the whole key area
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
void keynr_2_key(crack_task task, int key_nr, char **key)
{
	int char_nr = 0;
	int i = 0;
	int keylen = 0;

	if(*key == NULL)
		*key = (char*) calloc(task.keysize_max + 1, sizeof(char));
	
	if(key_nr == 0) 
	{
		*key[0] = '\0';
		return;
	}
	
	for(keylen = 0; key_nr >= pow(task.base, keylen); ++keylen)
		key_nr -= pow(task.base, keylen);
	
	do
	{
		char_nr = key_nr % task.base;
		key_nr = key_nr / task.base;
		(*key)[i] = task.charset[char_nr];
		i++;
	}
	while(key_nr != 0 || strlen(*key) != keylen);
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
		if(compare_hash(key, task->hash, task->algorithm) == 1)
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

//increase the key by one
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

//set a task to zero
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

// free all elements
void free_crack_task(crack_task* task)
{
	if(task->charset != NULL)	free(task->charset);
	if(task->hash != NULL)		free(task->hash);
	if(task->salt != NULL)		free(task->salt);
	if(task->start_key != NULL)	free(task->start_key);
}

//convert the key by algorithm and compare it with the given hash
bool compare_hash(char* key, char* hash, enum algo_num algo)
{
	char* key_hash = NULL;
	
	switch(algo)
	{
		case gnu_crypt: key_hash = (char*) crypt(key, hash); break;
		default: printf("Not implemented jet!\n"); break;
	}

	if(strncmp(key_hash, hash, strlen(key_hash)) == 0)
	{
		//printf("Key found: \"%s\" hash: \"%s\"\n\n", key, key_hash);
		return true;
	}
	
	return false;
}
