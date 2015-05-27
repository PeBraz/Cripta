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
*/
void
file_write(char * full_name, char * content)
{

	//open real file
	FILE * f = fopen(full_name, "r");

	char * file_name_suffix = "_cripta"
	int file_name_length = strlen(full_name);

	char * new_file_name = malloc(file_name_length + strlen(file_name_suffix) + 1);
	sprintf(new_file_name, "%s%s", full_name, file_name_suffix);
	//create virtual file
	FILE * new_f = fopen(new_file_name, "w+");

	//get size of file in bytes
	byte meta_size_text[2];	
	meta_size_text[0] = (file_name_length >> 8) & 0xFF;
	meta_size_text[1] = file_name_length & 0xFF;

	//write metadata
	fwrite(meta_size_text, sizeof(byte), 2, new_f);
	fwrite(full_name, sizeof(char), file_name_length, new_f);
	// write 256bit hash (32bytes)
	fwrite(get_test_hash(), sizeof(unsigned char), 32, new_f);
	//write file
	char * text = read_full_file(f);
	fwrite(text, sizeof(char), strlen(text), new_f);
	free(text);

}

unsigned char *
get_test_hash()
{
	return "00000000000000000000000000000000";
}

char *
read_full_file(FILE * f)
{
	fseek(f, SEEK_END);
	int length = fteel(f);
	rewind();
	char * data = malloc(length + 1);
	fread(data, sizeof(char), length, f);
	return data;
}