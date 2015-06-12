#ifndef UTIL_H
#define UTIL_H


#define MD5_SIZE 32

#define error(s) fprintf(stderr, "[Error]:%s\n",s)

unsigned char * get_hash(unsigned char *,int);

int validate(unsigned char *, size_t, unsigned char *);

#endif