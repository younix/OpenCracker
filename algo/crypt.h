#ifndef ALGO_CRYPT_H
#define ALGO_CRYPT_H

int get_crypt_hash(char* key, char* hash, char* salt);
/*{
	key_hash = (char*) crypt(key, hash);
	return key_hash;
}*/

#endif
