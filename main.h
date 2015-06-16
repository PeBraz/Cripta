#include <stdio.h>
#include <stdlib.h>
#include "list.h"

#ifndef _MAIN_H
#define _MAIN_H


struct directory
{
  char * name;
  List * directories;
  List * files;
};

typedef struct cripta_file
{
  char * name;
  unsigned char * hash;
  int content_offset;
  int content_size;

} cripta_file;


int is_dir(char*);

List * list_dir(char*);

void * string(char*);

char * read_full_file(char*,int*);

void file_write(char*);

void create_cripta(char*);

void create_cripta_with_father(struct directory *, FILE*);

char * create_dir_meta(struct directory*, int*);

unsigned char * int_to_bytes(int, int);

int bytes_to_int(unsigned char *, int);

void add_file_offset_meta(char *, int, int);

void add_dir_offset_meta(char *, int, int);

unsigned char * write_cripta_file(char * path, int * size);
#endif
