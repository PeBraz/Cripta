#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <stdarg.h>
#include "main.h"

#define error(s) printf("[Error]:%s\n",s)

char * concat_path(char*, char*);

struct directory
{
  char * name;
  List * directories;
  List * files;
};



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


struct directory *
new_dir(char * path)
{
  DIR * dir = opendir(path);
  if (dir == NULL)
    {
    error("unable to open path");
    return NULL;
    }

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
//  This should be used on the root, if only a subdirectory is freed, the root could still try 
//  and access a freed subdir and it would lead to a seg fault.
//
//  @param  dir  the directory to be freed   
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

//  
//
//

void
create_cripta(char * path)
{
  const struct directory * dir = new_dir(path);
  const char * file_name_suffix = "_CRIPTA";
  //create new file
  char * new_file_name = malloc(strlen(path) + strlen(file_name_suffix) + 1);
  sprintf(new_file_name, "%s%s", path, file_name_suffix);
  FILE * new_f = fopen(new_file_name, "w+");
  free(new_file_name);

  char * dir_meta = create_dir_meta(dir, NULL);
  fwrite(dir_meta, sizeof(char), strlen(dir_meta), new_f);

  ListNode * node = dir->directories->head;
  int pos = 0;
  char * sub_dir_meta;
  while (node != NULL)
    {
      sub_dir_meta = create_dir_meta((struct directory)node->data, dir_meta);
    }
  char * sub_dir_meta = create_dir_meta(dir_meta);
  char * file_meta = crate_file_meta(dir_meta);

  //For every directory write a meta and then for each file
  //try to create an auxiliar function to do this recursively

  directory_free(dir);
}


//
//  create_cripta_with_father:
//     Creating a cripta, will start by going down each directory, until it reaches
//     the directories' roots. Writing all the files and then the directory meta at the      
//     initial position of the file.
//  
//
void
create_cripta_with_father(struct directory dir,char * father_meta,
                          FILE * file)
{
  int dir_meta_position = ftell(file);

  const int father_meta_size = strlen(father_meta);
  fseek(file, father_meta_size, SEEK_CUR);  //reserve space for when writing meta

  struct directory curr_dir;
  ListNode * node = dir->directories->head;
  while (node != NULL)
    {
      curr_dir = (struct directory) node->data;
      char * son_meta = create_dir_meta(curr_dirr);
      create_cripta_with_father(curr_dirr, son_meta);
      free(son_meta)
      node = node->next;
    }

  node = dir->files->head;
  while (node != NULL)
    {
      //writes the file in the file and updates the directory meta that points to it
      write_cripta_file((char *) filename, father_meta);
    }

  //after all files written, write the directory meta fully updated
  fseek(file, dir_meta_position, SEEK_SET);
  fwrite(father_meta, sizeof(char), father_meta_size, file);
  fseek(file, 0, SEEK_END);
}



//
//  create_dir_meta - creates and reserves space for a directory meta
//
//  1 - size of the path string - 2 bytes
//  2 - path string - xbytes  (1byte each)
//  3 - number of directories - 2 bytes
//  4 - directories offset - xdirectories (4bytes each)
//  5 - number of files - 2 bytes
//  6 - files offset - xfiles (4bytes each)
//
//  @param    dir   the directory correspondent to the meta to create
//  @return         the meta as a string
//
char *
create_dir_meta(struct directory * dir)
{
  const int path_size_length = 2;
  const int dir_count_length = 2;
  const int file_count_length = 2;
  const int dir_name_length = 4;
  const int file_name_length = 4;

  const int path_name_length = strlen(dir->name);
  const int meta_size = path_size_length + path_name_length
          + dir_count_length + (dir_name_length * dir->directories->length)  
          + file_count_length + (file_name_length * dir->files->length)
          + 1;

  char * meta = malloc(meta_size);

  unsigned char[] b_path_name_length = int_to_bytes(path_name_length, 2);
  sprintf(meta,"%c%c%s",b_path_name_length[0], b_path_name_length[1], dir->name);
  free(b_path_name_length);

  unsigned char[] b_dir_count_length = int_to_bytes(dir_count_length, 2);
  strncat(meta, b_dir_count_length , 2);
  free(b_dir_count_length);
  ListNode * node = dir->directories->head;

  while(node!=NULL)
    { //reserve 4 bytes
      strcat(meta,"0000", 4);
      node = node->next;
    }

  unsigned char[] b_file_count_length = int_to_bytes(file_count_length, 2);
  strncat(meta, b_file_count_length , 2);
  free(b_file_count_length);

  node = dir->files->head;
  while(node!=NULL)
    { //reserve 4 bytes
      strcat(meta,"0000", 4);
      node = node->next;
    }

  meta[meta_size-1] = '\0';
  return meta;
}

//
// add_file_offset_meta - walks the meta string until it finds the position to store the 
//                        file offset
//
//  @param meta         where to store the offset
//  @param file_offset  the offset to write in the string in bytes
//  @param pos          the position in the files to store the offset
//
//  TODO: fix the magical numbers (they are byte sizes)
void
add_file_offset_meta(char * meta, int file_offset, int pos)
{

  int path_size =  bytes_to_int(meta,2);
  meta += 2 + path_size;
  int dir_count = bytes_to_int(meta,2);
  meta += 2 + (4*dir_count) + 2 + (4*pos);
  unsigned char[] b_file_offset = int_to_bytes(file_offset);
  strncpy(meta, b_file_offset, 4);
  free(b_file_offset);
}
//
// add_dir_offset_meta - walks the meta string until it finds the position to store the 
//                        directory offset
//
//  @param meta         where to store the offset
//  @param dir_offset   the offset to write in the string in bytes
//  @param pos          the position in the directories to store the offset
//
//
//  TODO: fix the magical numbers (they are byte sizes)
void
add_dir_offset_meta(char * meta, int dir_offset, int pos)
{
  int path_size =  bytes_to_int(meta,2);
  meta += 2 + path_size + 2 + (4*pos);
  unsigned char[] b_dir_offset = int_to_bytes(file_offset);
  strncpy(meta, b_dir_offset, 4);
  free(b_dir_offset);
}


//is this really needed ?
unsigned char[]
int_to_bytes(int integer, int num_of_bytes)
{
  if (num_of_bytes <= 0) 
    return NULL;

  unsigned char 2bytes_array = malloc(num_of_bytes);
  int i, shift = (num_of_bytes - 1) * 8;
  for (i = 0; i< num_of_bytes; i++)
    {
      bytes_array[i] = integer >> shift;
      shift--;
    }
  return bytes_array;
}

int
bytes_to_int(unsigned char[] bytes, int num_of_bytes)
{
  if (num_of_bytes <= 0) 
    return 0;

  int integer = 0;
  int i, shift = (num_of_bytes - 1) * 8;
  for (i = 0; i< num_of_bytes; i++)
    {
      integer = bytes[i];
      integer <<= shift;
    }
  return integer;

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