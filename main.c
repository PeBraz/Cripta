#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

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
  struct dirent * dp;
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

unsigned char *
get_test_hash()
{
  static unsigned char hash[] = "00000000000000000000000000000000";
  return hash;
}

char *
read_full_file(FILE * f)
{
  fseek(f, 0, SEEK_END);
  int length = ftell(f);
  rewind(f);
  char * data = malloc(length + 1);
  fread(data, sizeof(char),length, f);
  data[length] = '\0';
  return data;
}


/*
* First implementation, assumes input is not a full directory, but 1 file.
* needs to be able to write a file and encrypt it with metadata, 
* metadata:
*   - filenamelength (2bytes)
*   - filename  (max 65536 characters)
*   - HASH (will use 256 (2bytes))
*   total: 2bytes + filenamelength + 2bytes
*/

void
file_write(char * full_name)
{
  //open real file
  FILE * f = fopen(full_name, "r");

  if (f == NULL)
    {
    error("Unable to open file");
    return;
    }

  char * file_name_suffix = "_cripta";
  int file_name_length = strlen(full_name);

  char * new_file_name = malloc(file_name_length + strlen(file_name_suffix) + 1);
  sprintf(new_file_name, "%s%s", full_name, file_name_suffix);
  //create virtual file
  FILE * new_f = fopen(new_file_name, "w+");
  free(new_file_name);
  //get size of file in bytes
  unsigned char meta_size_text[2]; 
  meta_size_text[0] = (file_name_length >> 8) & 0xFF;
  meta_size_text[1] = file_name_length & 0xFF;

  //write metadata
  fwrite(meta_size_text, sizeof(unsigned char), 2, new_f);
  fwrite(full_name, sizeof(char), file_name_length, new_f);
  // write 256bit hash (32bytes)
  unsigned char * hash = get_test_hash();
  fwrite(hash, sizeof(unsigned char), 32, new_f);
  //write file
  char * text = read_full_file(f);
  fwrite(text, sizeof(char), strlen(text), new_f);
  free(text);
  fclose(f);
  fclose(new_f); 
}

int
main(int argc, char * argv[])
{
  /*if (argc < 2)
    {
      error("No Directory path given, exiting now.");
      return EXIT_FAILURE;
    }
  List * l = list_dir("/");

  ListNode * node = l->head;
  while (node != NULL)
    {
      printf("%s\n", (char*)node->data);
      node = node->next;
    }
  list_free(l); */

  file_write("README.md");  
  return EXIT_SUCCESS;
}