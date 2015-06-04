#ifndef UTIL_H
#define UTIL_H

unsigned char * get_hash(char *,int*);

void file_write(char *, char *);

unsigned char * get_test_hash();

char * read_full_file(FILE *);

#endif