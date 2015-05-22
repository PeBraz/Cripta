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
  return stat(path, &s) == 0 && (s.st_mode & S_IFDIR)
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
