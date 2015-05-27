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


walk_s * walk(char*);

int is_dir(char*);

List * list_dir(char*);

void * string(char*);

char * read_full_file(FILE*);

void file_write(char*);

#endif
