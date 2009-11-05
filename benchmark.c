#include <time.h>
#include <stdio.h>

#include <openssl/md5.h>
#include <stdlib.h>
#include <string.h>

#define TESTDATA "Das sind meine Testdaten!"

int main()
{
	unsigned char* data;	
	unsigned char* md5str;
	time_t end = time(NULL);
	long long int steps = 0;
	
	md5str = (char*) calloc( 20,sizeof(char));
	data = (unsigned char*) calloc( strlen(TESTDATA) + 1, sizeof(unsigned char));
	strncpy(data, TESTDATA, strlen(TESTDATA));


	for(	time_t start = time(NULL); 
		(end - start <= 5); 
		end = time(NULL))
	{
		MD5( data, strlen(TESTDATA), md5str);
		//printf("MD5: %s\n", md5str);
		++steps;
	}

	for(int i = 0; i < 16; ++i)
		printf("D: %d\n", md5str[i]);

	printf("steps: %lld\n", steps);

	return 0;
}
