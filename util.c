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

#include <openssl/md5.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
	for(i = 0; i < md_len; i++)
	        printf("%02x", md_value[i]);
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

	Openssl_add_all_digests();

	const EVP_MD * md = EVP_sha256();
	unsigned char * hash = malloc(EVP_MAX_MD_SIZE);

	EVP_MD_CTX *mdctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mcdtx, md, NULL);
	EVP_DigestUpdate(mdctx, msg, strlen(msg));
	EVP_DigestFinal(mdctx, hash,length);
	EVP_cleanup();

	return  hash;
}
/*
*	First implementation, assumes input is not a full directory, but 1 file.
*	needs to be able to write a file and encrypt it with metadata, 
*	metadata:
*		- filenamelength (2bytes)
*		- filename	(max 65536 characters)
*		- HASH (will use 256 (2bytes))
*		total: 2bytes + filenamelength + 2bytes
*
*/
void
file_write(char * full_name, char * content)
{

	//open real file
	FILE * f = fopen(full_name, "r");

	char * file_name_suffix = "_cripta"
	int file_name_length = strlen(full_name);
	char * new_file_name = malloc(file_name_length + strlen())

	//create virtual file
	FILE * new_f = fopen(full_n)

	byte length[2];	
	length[0] = (file_name_length >> 8) & 0xFF;
	length[1] = file_name_length & 0xFF;

	fwrite();


}