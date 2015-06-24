#ifndef UTIL_H
#define UTIL_H


#define MD5_SIZE 32
#define DO_ENCRYPT 1
#define DO_DECRYPT 0


#define error(s) fprintf(stderr, "[Error]:%s\n",s)

unsigned char * get_hash(unsigned char *,int);

int validate(unsigned char *, size_t, unsigned char *);

int do_crypt(FILE * content, int length , FILE * out, char * password, int do_encrypt);

#endif