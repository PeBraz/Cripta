//#include <openssl/md5.h>
//#include <string.h>
/*
unsigned char * MD5(const unsigned char * d, unsigned long n,
					 unsigned char * md)
*/
//unsigned char * hash = malloc(sizeof(MD5_DIGEST_LENGTH));
//MD5( strlen(msg), msg,hash)
//free(hash);

//////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include <openssl/evp.h>

int
main()
{


	char * scan_msg; 
	puts("Write a test message:\n");
	scanf("%s", scan_msg);

	char * msg = malloc(strlen(scan_msg) + 1);	//free
	strcpy(msg, scan_msg);

	int len;
	unsigned char * hash = get_hash(msg, &len);

	printf("Digest for msg is: ");
	int i;
	for(i = 0; i < len; i++)
	        printf("%02x", hash[i]);

	printf("\n");

	free(msg);
	free(hash);
	return 0;
}
/**
*	SHA1 to be used for single file and full file verification
*
**/
unsigned char *
get_hash(char * message, int * length)
{

	OpenSSL_add_all_digests();

	const EVP_MD * md = EVP_sha256();
	unsigned char * hash = malloc(EVP_MAX_MD_SIZE);

	EVP_MD_CTX *mdctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, message, strlen(message));
	EVP_DigestFinal(mdctx, hash,length);
	EVP_cleanup();

	return  hash;
}


unsigned char *
get_test_hash()
{
	return "00000000000000000000000000000000";
}

char *
read_full_file(FILE * f)
{
	fseek(f, 0, SEEK_END);
	int length = ftell(f);
	rewind(f);
	char * data = malloc(length + 1);
	fread(data, sizeof(char), length, f);
	return data;
}