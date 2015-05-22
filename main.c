#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define error(s) printf("[Error]: %s\n",s)
#define not(cond) (!cond)


#ifdef (__unix__)
#define NIX __unix__
#include "dirent.h"
#elif (WIN32) || (_WIN32)
#define WINDOWS (WIN32) || (_WIN32)
#else
#endif

//Cripta main

struct walk_s
{
  char * fullpath;
  char * dirs[];
  char * files[];
}

//
//  walk - returns the information, related to the given path
//
//  @param  path  to walk
//  @return       a walk_s containing the full path of the given path
//                the directories inside the directory
//                the files inside the directory

walk_s *
walk(char * path)
{
  if not(is_dir(path))
    {
      error("Path given is not a directory.")
      return NULL;
    }

#ifdef NIX

  DIR * dir = opendir(path);
  if (dir == NULL)
    {
    error("unable to open path");
    return NULL;
    }

  dirent * dp;
  while ((dp = readdir(dir)) != NULL)
    {
      

    }
#elif WINDOWS

#endif

}

//
// is_dir - tests if given location is a directory
//
//  @param  path  path to test
//  @return       value of path pointing to a directory
//
int
is_dir(char * path)
{
  struct stat s;
  return stat(path, &s) == 0 && (s.st_mode & S_IFDIR);
}

//
//  list_dir - returns all directories and files inside the directory
//             in the path given.
//
//  @param  path  path to list
//  @return       array with the name of all the items inside
//
char **
list_dir(char * path)
{


}


int
main(int argc, char * argv[])
{
  if (argc < 2)
    {
      error("No Directory path given, exiting now.");
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
