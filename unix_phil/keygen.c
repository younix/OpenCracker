//#define _GNU_SOURCE
#define _XOPEN_SOURCE 500
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <tgmath.h>
#include <ctype.h>

#define END_OF_KEYRANGE -1

void usage(char*);
void keynr_2_key(uint32_t);
uint32_t keyrange(void);
int get_next_key(int);
int ben_next_key(void);

char *charset;
char *key;
uint32_t base;
uint32_t keylength_max;
uint32_t start;
uint32_t end;

int main(int argc, char** argv)
{
	keylength_max = 0;
	start = 0;
	end = 0;

	int option = 0;
	int optionindex = 0;
	// define command line options
	struct option long_options[] =
	{
		{"help"         , 0, NULL, 'h'},
		{"charset"      , 1, NULL, 'c'},
		{"length"       , 1, NULL, 'l'},
		{"start"	, 1, NULL, 's'},
		{"end"		, 1, NULL, 'e'},
		{0, 0, 0, 0}
	};

	// analysing command line options
	while((option = getopt_long( argc, argv, "hc:l:s:e:", long_options, &optionindex)) != -1)
	{
		switch (option)
		{
			case 'c':
				charset = strdup(optarg);
				break;
			case 'l':
				for(int i = 0; i < strlen(optarg); ++i)
					if(isdigit(optarg[i]) == 0)
						exit(-1);

				keylength_max = atoi(optarg);
				break;
			case 's':
				for(int i = 0; i < strlen(optarg); ++i)
					if(isdigit(optarg[i]) == 0)
						exit(-1);

				start = atoi(optarg);
				break;
			case 'e':
				for(int i = 0; i < strlen(optarg); ++i)
					if(isdigit(optarg[i]) == 0) {
						usage(argv[0]);
						exit(-1);
					}

				end = atoi(optarg);
				break;
			case 'h':
			default:
				usage(argv[0]);
				exit(1);
				break;
		}
	}

	if (charset == NULL) {
		usage(argv[0]);
		exit(1);
	}

	base = strlen(charset);

	if (end == 0)
		end = keyrange();

	if (start > end) {
		uint32_t tmp = end;
		end = start;
		start = tmp;
	}

	keynr_2_key(start);
	
	for (uint32_t i = start; i < end; ++i) {
		printf("%s\n", key);
		//get_next_key(0);
		ben_next_key();
	}

	return -1;
}

void usage(char* prog) {
	printf( "usage: %s -c [OPTION...]\n"
		"	-c, --charset	charset of the key\n"
		"	-l, --lenght	maximum lenght of the key\n"
		"	-s, --start	position of the first key\n"
		"	-e, --end	position of the last key\n"
		"	-h, --help	this help text\n"
	, prog);
}

// convert a position of a key in the keyrange to the spezific key
void keynr_2_key(uint32_t key_nr)
{
	int char_nr = 0;
	int i = 0;
	int keylen = 0;

	if(key == NULL)
		key = (char*) calloc(keylength_max + 1, sizeof(char));

	if(key_nr == 0) {
		key[0] = '\0';
		return;
	}

	for (keylen = 0; key_nr >= pow(base, keylen); ++keylen)
		key_nr -= pow(base, keylen);

	do {
		char_nr = key_nr % base;
		key_nr = key_nr / base;
		key[i] = charset[char_nr];
		i++;
	} while (key_nr != 0 || strlen(key) != keylen);
}

//calculate the keyrange
uint32_t keyrange(void)
{
	int i;
	long long int keyrange = 0;
	for(i = 0; i <= keylength_max; ++i)
		keyrange += pow(base, i);

	return keyrange;
}

//increase the key by one
int get_next_key(int pos)
{
	int i = 0;

	for (i = 0; i < base; ++i)
	{
		if (key[pos] == charset[i])
		{
			if ((i + 1) < base)
			{
				key[pos] = charset[i+1];
				return 0;
			}
			else
			{
				key[pos] = charset[0];
				if ((pos + 1) < keylength_max)
					return get_next_key(pos + 1);
				else
					return END_OF_KEYRANGE;
			}
		}
	}

	key[pos] = charset[0];

	return 0;
}

//ben's version of next string
int ben_next_key(void) {
	int i;
	int l = strlen(key);
	for (i = 0; i < l; ++i) { 
		if (key[i] == charset[base-1]) {
			if (i >= keylength_max-1)
				return -1;
			key[i] = charset[0];
			if (i == l-1) {
				key[i+1] = charset[0];
				// should be ensured by calloc, but safety first!
				key[i+2] = '\0';
				// not needed (for loop would end NOW also without this) - but for readability of the algorithm
				break;
			}
		} else {
			key[i] = *(strchr(charset, key[i])+1);
			break;
		}
	}
	// just a little feature, if a empty key is given
	// the first character in the base is used as first key
	if (l == 0)
		key[0] = charset[0];
	return 0;
}
