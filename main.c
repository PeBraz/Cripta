#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <stdarg.h>
#include "main.h"

#define error(s) printf("[Error]:%s\n",s)
#define not(cond) (!cond)

char * concat_path(char*, char*);

struct directory
{
  char * name;
  List * directories;
  List * files;
};


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

  List * files_lst = list_new(int(*)(char *, char*)strcmp);
  List * dir_lst = list_new(int(*)(char *, char*)strcmp);

  List * path_lst = list_dir(path);
  ListNode * node = path_lst->head;
  while(node != NULL)
      list_add((is_dir(path) ? dir_lst : files_lst), path_lst->data);

  list_free(path_list);


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
// is_file - tests if given location is a file
//  @param  path  path to test
//  @return       value of path pointing to a file
//
int
is_file(char * path)
{
  return access(path, F_OK) != -1;
}

//
//  list_dir - returns all directories and files inside the directory
//             in the path given.
//
//  @param  path  in the directory
//  @return       a directory structure with all the items inside
//
/*
struct directory *
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
*/

struct directory *
new_dir(char * path)
{
  DIR * dir = opendir(path);
  if (dir == NULL)
    {
    error("unable to open path");
    return NULL;
    }
  //  printf("I am path:%s\n", path );
  //define my_path so that subdirectories can know their relative paths


  struct directory * virt_dir = malloc(sizeof(struct directory));

  virt_dir->directories = list_new(NULL);
  virt_dir->files = list_new((int(*)(const void*, const void*))strcmp);
  virt_dir->name = malloc(strlen(path) + 1);
  strcpy(virt_dir->name, path);

  char * next_path;
  struct dirent * dp;
  while ((dp = readdir(dir)) != NULL)
    {

  
    if (!strcmp(".",dp->d_name) || !strcmp("..",dp->d_name)) continue;
    //  If a directory is found, a recursive call will start for new_dir(), until there are no more
    // directories (symbolic links may be bad, because this won't account for cycles in the file system)
    next_path = concat_path(path, dp->d_name);
    //  printf("found: %s is a %s\n", next_path, 
    //      is_dir(next_path) ? "Directory": 
    //                           is_file(next_path) ? "File" : "DAFUQ");
  

    if (is_dir(next_path)) 
      list_add(virt_dir->directories, new_dir(next_path));
    else if (is_file(next_path))
      list_add(virt_dir->files, string(next_path));

    free(next_path);
    }

  closedir(dir);
  return virt_dir;
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

//
// directory_free - frees any data allocated by a directory struct
//    This processes the directory recursively, so that any subdirectory is also freed
//
//  @param  dir  the directory to be freed
//  @return       
//
void
directory_free(struct directory * dir)
{
  ListNode * node = dir->directories->head;
  while (node != NULL){
    directory_free(node->data);
    node = node->next;
  }
  free(dir->name);
  list_full_free(dir->files);
  list_free(dir->directories);
  free(dir);
}

//  
//  concat_path - concatenates a relative path with a directory name
//        the path prefix and sufix provided need to be destroyed by the user
//    
//  @param  path_prefix   relative path
//  @param  path_sufix    of the path to be concatenated
//  @return               the concatenated path
//
char * concat_path(char * path_prefix, char * path_sufix)
{
  const char * PATH_SEP = "/";
  char * full_path = malloc(strlen(path_prefix) + strlen(PATH_SEP) + 
                            strlen(path_sufix) + 1);
  sprintf(full_path,"%s%s%s", path_prefix, PATH_SEP, path_sufix);
  return full_path;
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
  if (argc < 2)
    {
      error("No Directory path given, exiting now.");
      return EXIT_FAILURE;
    }
  struct directory * s = new_dir(".");
  directory_free(s);
 /* if (l == NULL)

  ListNode * node = l->head;
  while (node != NULL)
    {
      printf("%s\n", (char*)node->data);
      node = node->next;
    }
  list_full_free(l); 
*/
  //file_write("README.md");  
  return EXIT_SUCCESS;
}