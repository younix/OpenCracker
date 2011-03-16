#define _XOPEN_SOURCE 700
#include <string.h>
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
	keysize_max = 0;
	start = 0;
	end = 0;

	int option = 0;

	/* analysing command line options */
	while((option = getopt(argc, argv, "hc:l:s:e:")) != -1)
	{
		switch (option)
		{
			case 'c':
				charset = strdup(optarg);

				if (charset == NULL)
					goto error;
				break;
			case 'l':
				keysize_max = strtol(optarg, NULL, 0);
				
				if (keysize_max < 1)
					goto error;
				
				break;
			case 's':
				start = strtol(optarg, NULL, 0);
				
				if (start < 0)
					goto error;
				break;
			case 'e':
				end = strtol(optarg, NULL, 0);

				if (end < 0)
					goto error;
				break;
			case 'h':
			default:
				goto error;
				break;
		}
	}

	if (charset == NULL)
		goto error;

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

	exit(EXIT_SUCCESS);
error:
	printf(	"usage: %s -l LENGTH -c CHARSET [OPTIONS]\n\n"
		"LENGTH\n"
		"	max. length of the key\n\n"
		"CHARSET\n"
		"	charset of the generated keys\n\n"
		"OPTIONS\n"
		"	-s NUMBER	start position (inside of the keyrange)\n"
		"	-e NUMBER	end position\n",
		argv[0]);
	usage();
	exit(EXIT_FAILURE);
}

void usage(void) {
}

/* set the key to the position of key_nr */
void keynr_2_key(uint32_t key_nr) {
	int char_nr = 0;
	uint32_t i = 0;
	uint32_t keylen = 0;

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

/* calculate the keyrange */
uint32_t keyrange(void) {
	uint32_t keyrange = 0;

	for(uint32_t i = 0; i <= keysize_max; ++i)
		keyrange += pow(base, i);

	return keyrange;
}

/* increase the key by one */
int get_next_key(int pos) {
	for (int i = 0; i < base; ++i) {
		if (key[pos] == charset[i]) {
			if ((i + 1) < base) {
				key[pos] = charset[i+1];

				return 0;
			} else {
				key[pos] = charset[0];

				if ((pos + 1) < keysize_max)
					return get_next_key(pos + 1);
				else
					return END_OF_KEYRANGE;
			}
		}
	}

	key[pos] = charset[0];

	return 0;
}

/* ben's version of next key */
int ben_next_key(void) {
	int l = strlen(key);
	for (int i = 0; i < l; ++i) { 
		if (key[i] == charset[base-1]) {
			if (i >= keylength_max-1)
				return -1;
			key[i] = charset[0];
			if (i == l-1) {
				key[i+1] = charset[0];
				/* should be ensured by calloc, but safety first! */
				key[i+2] = '\0';
				/* not needed (for loop would end NOW also without this)
				 * but for readability of the algorithm */
				break;
			}
		} else {
			key[i] = *(strchr(charset, key[i])+1);
			break;
		}
	}
	/* just a little feature, if a empty key is given
	 * the first character in the base is used as first key */
	if (l == 0)
		key[0] = charset[0];
	return 0;
}
