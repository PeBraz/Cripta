#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <time.h>

#include "util.h"

#define RAND_PART_SIZE 10


/**
*	SHA2 to be used for single file and full file verification
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

	return hash;
}

//
//	rand_number_as_string - generates a variable sized number as a string, 
//		all characters will be ascii numbers
//	
//	WARNING: initialize srand before using this
//	@param	size 	of the string to be generated
//	@return			generated random string	
//

char *
rand_number_as_string(int size)
{	
  char * part = malloc(RAND_PART_SIZE + 1);
  char * full_string = malloc(size + 1);
  strcpy(full_string,"");
  int off, i;
  //do 10 padding (RAND_MAX can have up to 10 houses)
  for (i=size, off=0; i > 0; off+=RAND_PART_SIZE, i -= RAND_PART_SIZE)
    {
  	sprintf(part,"%010u",rand());
  	memcpy(full_string + off, part, (i > RAND_PART_SIZE) ? RAND_PART_SIZE : i);
    }
  full_string[size] = '\0';
  free(part);
  return full_string;
}



unsigned char *
encrypt_content(char * content, int length)
{





}
//http://stackoverflow.com/questions/12524994/encrypt-decrypt-using-pycrypto-aes-256
//taken from https://www.openssl.org/docs/crypto/EVP_EncryptInit.html
int do_crypt(char * in, char * out, int do_encrypt, char * passphrase)
        {
        /* Allow enough space in output buffer for additional block */
        unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
        srand((unsigned int)time(NULL));

        int inlen, outlen;
        EVP_CIPHER_CTX ctx;
        /* Bogus key and IV: we'd normally set these from
         * another source.
         */
        unsigned char key[] = "0123456789abcdeF";
        unsigned char iv[] = "1234567887654321";

        /* Don't set key or IV right away; we want to check lengths */
        EVP_CIPHER_CTX_init(&ctx);
        EVP_CipherInit_ex(&ctx, EVP_aes_128_cbc(), NULL, NULL, NULL,
                do_encrypt);
        OPENSSL_assert(EVP_CIPHER_CTX_key_length(&ctx) == 16);
        OPENSSL_assert(EVP_CIPHER_CTX_iv_length(&ctx) == 16);

        /* Now we can set key and IV */
        EVP_CipherInit_ex(&ctx, NULL, NULL, key, iv, do_encrypt);

        for(;;)
                {
                inlen = fread(inbuf, 1, 1024, in);
                if(inlen <= 0) break;
                if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen))
                        {
                        /* Error */
                        EVP_CIPHER_CTX_cleanup(&ctx);
                        return 0;
                        }
                fwrite(outbuf, 1, outlen, out);
                }
        if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen))
                {
                /* Error */
                EVP_CIPHER_CTX_cleanup(&ctx);
                return 0;
                }
        fwrite(outbuf, 1, outlen, out);

        EVP_CIPHER_CTX_cleanup(&ctx);
        return 1;
        }