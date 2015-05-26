#include <stdio.h>
#include <stdlib.h>

#ifndef _MAIN_H
#define _MAIN_H

typedef struct walk_s
{
  char * fullpath;
  List * dirs;
  List * files;
} walk_s;


walk_s * walk(char * path);

int is_dir(char * path);

List * list_dir(char * path);

void * string(char * old_str);


#endif
