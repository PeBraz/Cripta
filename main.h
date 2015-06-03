#include <stdio.h>
#include <stdlib.h>
#include "list.h"

#ifndef _MAIN_H
#define _MAIN_H

typedef struct walk_s
{
  char * fullpath;
  List * dirs;
  List * files;
} walk_s;

struct directory
{
  char * name;
  List * directories;
  List * files;
};


walk_s * walk(char*);

int is_dir(char*);

List * list_dir(char*);

void * string(char*);

char * read_full_file(FILE*);

void file_write(char*);

void create_cripta(char*);

void create_cripta_with_father(struct directory *,
							   char*, FILE*);

char * create_dir_meta(struct directory*);

unsigned char * int_to_bytes(int, int);

int bytes_to_int(unsigned char *, int);

#endif
