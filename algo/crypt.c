#include "crypt.h"

int get_crypt_hash(char* key, char* hash, char* salt)
{
	key_hash = (char*) crypt(key, hash);
	return key_hash;
}
