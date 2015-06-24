#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <time.h>

#include "util.h"
#include "main.h"

#define RAND_PART_SIZE 10
#define AES256_BLOCK_SIZE 32
#define AES256_IV_SIZE 16
#define KBBLOCK 1024

//
//	get_hash - md5 to be used for single file and full file verification
//
//	@param message	to calculate the md5 hash from, can be binary
//	@param length 	initial length of the message
//
//	@return					the md5 hash of 32 bytes
//
unsigned char *
get_hash(unsigned char * message, int  length)
{

	OpenSSL_add_all_digests();

	int p;	//just to provide a pointer for the digest length, useless
	const EVP_MD * md = EVP_md5();
	unsigned char * hash = malloc(EVP_MAX_MD_SIZE);

	EVP_MD_CTX *mdctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, message, length);
	EVP_DigestFinal(mdctx, hash,&p);
	EVP_MD_CTX_destroy(mdctx);
	EVP_cleanup();

	return hash;
}

//
//	validate - compares the file with the respective hash, to confirm the file is valid
//
//	
//


int validate(unsigned char * file, size_t file_size, unsigned char * md5)
{
	unsigned char * hash = get_hash(file, file_size);
  int result = memcmp(hash, md5, MD5_SIZE);
  free(hash);
  return !result;
}



//
//  aes256_key_and_iv - given a password create a 32 byte string and 16 byte string
//              to be used by aes256
//
//  @param password   of the user
//  @param key        unused pointer 
//  @param iv         unused pointer 
//  @http://stackoverflow.com/questions/9488919/openssl-password-to-key  
//
void
aes256_key_and_iv(const char * password, char * key,
                  char * iv)
{
  const EVP_CIPHER *cipher;
  const EVP_MD *dgst = NULL;
  const unsigned char *salt = NULL;

  key = malloc(32);
  iv = malloc(16);

  OpenSSL_add_all_algorithms();

  cipher = EVP_get_cipherbyname("aes-256-cbc");
  if (!cipher) 
    error("no such cipher");

  dgst = EVP_get_digestbyname("md5");
  if (!dgst)
    error("no such digest");

  int success = EVP_BytesToKey(cipher, dgst, salt, (unsigned char *) password,
                               strlen(password), 1, key, iv);
  if (!success)
    error("EVP_BytesToKey failed");
}

//
//  Takes content from a file and writes it into another file
//  (taken from: https://www.openssl.org/docs/crypto/EVP_EncryptInit.html)
//  Reads only the length from the file starting from its current position
// 
//  @param  content    file to take (un)encrypted content from
//  @param  length     of the file, if -1, read until the end
//  @param  out        file to append (un)encrypted content to
//  @param  password   for key and iv generating
//  @param  do_encrypt specify if you want to encrypt (1) or decrypt (0)
//
//  @return            the number of bytes written to the out file
//                     if -1 is returned instead, then an error occured

int
do_crypt(FILE * content, int length , FILE * out, char * password, int do_encrypt)
{
  unsigned char inbuf[KBBLOCK], outbuf[KBBLOCK + EVP_MAX_BLOCK_LENGTH];

  int inlen, outlen, bytes_written_count = 0;
  EVP_CIPHER_CTX ctx;

  char * key;
  char * iv;

  if (length == -1)
    {
      int position = ftell(content);
      fseek(content, 0, SEEK_END);
      length = ftell(content) - position;
      fseek(content, SEEK_SET, position);
    }


  aes256_key_and_iv(password, key, iv);

  EVP_CIPHER_CTX_init(&ctx);
	EVP_CipherInit_ex(&ctx, EVP_aes_256_cbc(), NULL, NULL, NULL, do_encrypt);
  OPENSSL_assert(EVP_CIPHER_CTX_key_length(&ctx) == AES256_BLOCK_SIZE);
  OPENSSL_assert(EVP_CIPHER_CTX_iv_length(&ctx) == AES256_IV_SIZE);

	EVP_CipherInit_ex(&ctx, NULL, NULL, key, iv, 1);

	int offset = 0;
	for( ; length > 0; length -= KBBLOCK)
  	{
    inlen = fread(inbuf, sizeof(unsigned char), (length < KBBLOCK ? length : KBBLOCK), content);

    if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
 			{
			EVP_CIPHER_CTX_cleanup(&ctx);
      free(key);
      free(iv);
      return -1;
      }
   	fwrite(outbuf, sizeof(unsigned char), outlen, out);
    bytes_written_count += outlen;
    }

  if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen))
  	{           
    EVP_CIPHER_CTX_cleanup(&ctx);
    free(key);
    free(iv);
    return -1;
    }

  fwrite(outbuf, sizeof(unsigned char), outlen, out);
  bytes_written_count += outlen;

  EVP_CIPHER_CTX_cleanup(&ctx);
  free(key);
  free(iv);
  return outlen;
}
