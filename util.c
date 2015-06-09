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
int validate(unsigned char * file, unsigned char * md5, size_t file_size )
{
	char * hash = get_hash(file, file_size);
  int result = memcmp(hash, md5, MD5_SIZE);
  free(hash);
  return result;
}



//  srand((unsigned int)time(NULL)); before this
// length must have the initial content length and will return the new value (need to see if encryption changes length)
// is it block size dependent?
/*unsigned char *	
encrypt_content(char * content, int * length)
{
  unsigned char inbuf[KBBLOCK], outbuf[KBBLOCK + EVP_MAX_BLOCK_LENGTH];

  int size = *length;
  // size of a 1024 block + last block that is bigger + iv
  unsigned char * encrypted_content = malloc((size - 1)* KBBLOCK 
  																					+ KBBLOCK + EVP_MAX_BLOCK_LENGTH 
  																					+ AES256_IV_SIZE);

  int inlen, outlen;
  EVP_CIPHER_CTX ctx;

  unsigned char key[EVP_MAX_KEY_LENGTH];
	unsigned char iv[EVP_MAX_IV_LENGTH];


  EVP_CIPHER_CTX_init(&ctx);
	EVP_CipherInit_ex(&ctx, EVP_aes_256_cbc(), NULL, NULL, NULL, do_encrypt);
  OPENSSL_assert(EVP_CIPHER_CTX_key_length(&ctx) == AES256_BLOCK_SIZE);
  OPENSSL_assert(EVP_CIPHER_CTX_iv_length(&ctx) == AES256_IV_SIZE);

	EVP_CipherInit_ex(&ctx, NULL, NULL, key, iv, 1);

	int offset = 0;
	for(;;)
  	{
  	memcpy(inbuf, content, length < KBBLOCK? length :KBBLOCK);	

    if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
 			{
			EVP_CIPHER_CTX_cleanup(&ctx);
      return NULL;
      }
   	memcpy(encrypted_content ,outbuf, outlen);
   	fwrite(outbuf, 1, outlen, out);
  }
  if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen))
  	{
               
                EVP_CIPHER_CTX_cleanup(&ctx);
                return 0;
    }
  fwrite(outbuf, 1, outlen, out);

        EVP_CIPHER_CTX_cleanup(&ctx);
        return 1;


}
*/
//
//	aes256_key_and_iv - given a password create a 32 byte string and 16 byte string
// 							to be used by aes256
//
//	@param password 	of the user
//	@param key 				pointer to a 32 bytes allocated space
//	@param iv 				pointer to a 16 bytes allocated space
//	@http://stackoverflow.com/questions/9488919/openssl-password-to-key  
//
/*void
aes256_key_and_iv(const char * password, char * key,
									char * iv)
{
  const EVP_CIPHER *cipher;
  const EVP_MD *dgst = NULL;
  const unsigned char *salt = NULL;

  OpenSSL_add_all_algorithms();

  cipher = EVP_get_cipherbyname("aes-256-cbc");
  if (!cipher) 
   	error("no such cipher");

  dgst = EVP_get_digestbyname("md5");
  if (!dgst)
  	error("no such digest")

  int success = EVP_BytesToKey(cipher, dgst, salt, (unsigned char *) password,
        						 					 strlen(password), 1, key, iv))
 	if (!success)
    error("EVP_BytesToKey failed");
}

//http://stackoverflow.com/questions/12524994/encrypt-decrypt-using-pycrypto-aes-256
//taken from https://www.openssl.org/docs/crypto/EVP_EncryptInit.html
int do_crypt(char * in, char * out, int do_encrypt, char * passphrase)
        {

        unsigned char inbuf[KBBLOCk], outbuf[KBBLOCK + EVP_MAX_BLOCK_LENGTH];
        srand((unsigned int)time(NULL));

        int inlen, outlen;
        EVP_CIPHER_CTX ctx;

        unsigned char key[EVP_MAX_KEY_LENGTH];
        unsigned char iv[EVP_MAX_IV_LENGTH];

       
        EVP_CIPHER_CTX_init(&ctx);
        EVP_CipherInit_ex(&ctx, EVP_aes_128_cbc(), NULL, NULL, NULL,
                do_encrypt);
        OPENSSL_assert(EVP_CIPHER_CTX_key_length(&ctx) == 16);
        OPENSSL_assert(EVP_CIPHER_CTX_iv_length(&ctx) == 16);

        EVP_CipherInit_ex(&ctx, NULL, NULL, key, iv, do_encrypt);

        for(;;)
                {
                inlen = fread(inbuf, 1, 1024, in);
                if(inlen <= 0) break;
                if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
                        {
                 
                        EVP_CIPHER_CTX_cleanup(&ctx);
                        return 0;
                        }
                fwrite(outbuf, 1, outlen, out);
                }
        if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen))
                {
         
                EVP_CIPHER_CTX_cleanup(&ctx);
                return 0;
                }
        fwrite(outbuf, 1, outlen, out);

        EVP_CIPHER_CTX_cleanup(&ctx);
        return 1;
        }

*/