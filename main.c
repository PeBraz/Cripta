#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "list.h"
#include "main.h"

#define error(s) printf("[Error]: %s\n",s)
#define not(cond) (!cond)





//
//  walk - returns the information, related to the given path
//
//  @param  path  to walk
//  @return       a walk_s containing the full path of the given path
//                the directories inside the directory
//                the files inside the directory
/*
walk_s *
walk(char * path)
{
  if not(is_dir(path))
    {
      error("Path given is not a directory.");
      return NULL;
    }



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


}
*/
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
List *
list_dir(char * path)
{
  List * l = list_new((int(*)(const void*, const void*))strcmp);
  DIR * dir = opendir(path);
  if (dir == NULL)
    {
    error("unable to open path");
    return NULL;
    }
  dirent * dp;
  while ((dp = readdir(dir)) != NULL)
    list_add(l, string(dp->d_name));

  closedir(dir);

  return l;
}

void *
string(char * old_str)
{
  char * str = malloc((strlen(old_str) + 1) * sizeof(char));
  strcpy(str, old_str);
  return str;
}

int
main(int argc, char * argv[])
{
  if (argc < 2)
    {
      error("No Directory path given, exiting now.");
      return EXIT_FAILURE;
    }
  List * l = list_dir(".");
  printf("%d\n", l->length);
  return EXIT_SUCCESS;
}
