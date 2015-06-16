#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <stdarg.h>
#include <time.h>

#include "main.h"
#include "util.h"


//num of bytes reserved for writing a meta file
/* full space used to write the path name */
#define PATH_SIZE_LENGTH 2 
/* space used to write the number of directories/files*/
#define DIR_COUNT_LENGTH 2  
#define FILE_COUNT_LENGTH 2 
/* bytes used to write the offset to a directory/file within the cripta*/
#define DIR_OFFSET_LENGTH 4
#define FILE_OFFSET_LENGTH 4
/* bytes used for the length of the name of a directory/file */
#define DIR_NAME_LENGTH 2
#define FILE_NAME_LENGTH 2
/* bytes used for the length of the body of al file */
#define FILE_SIZE_LENGTH 4
/* number of bits in a byte */
#define BYTE 8 

/* paddings to be added to Directory and file, needs to have same size
 * as DIR_OFFSET_LENGTH and FILE_OFFSET_LENGTH
 */
#define DIR_OFFSET_PADDING "0000"
#define FILE_OFFSET_PADDING "0000"
char * concat_path(char*, char*);

void cripta_file_free(cripta_file * cf);


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


char *
read_full_file(char * filename, int * length)
{
  FILE * f = fopen(filename, "r");
  fseek(f, 0, SEEK_END);
  *length = ftell(f);
  rewind(f);
  char * data = malloc(*length + 1);
  fread(data, sizeof(char),*length, f);
  fclose(f);
  data[*length] = '\0';
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

void
directory_free_with_files(struct directory * dir)
{
  ListNode * node = dir->directories->head;
  while (node != NULL){
    directory_free_with_files(node->data);
    node = node->next;
  }
  free(dir->name);
  node = dir->files->head;
  while (node != NULL)
    {
      cripta_file_free(node->data);
      node = node->next;
    }
  list_free(dir->files);
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
char * 
concat_path(char * path_prefix, char * path_sufix)
{
  const char * PATH_SEP = "/";
  char * full_path = malloc(strlen(path_prefix) + strlen(PATH_SEP) + 
                            strlen(path_sufix) + 1);
  sprintf(full_path,"%s%s%s", path_prefix, PATH_SEP, path_sufix);
  return full_path;
}

//
//  path_leaf - returns the last element in the path, pointing to the path after the last '/''
//
char *
path_leaf(char * path)
{
  int i;
  for (i = strlen(path); i >= 0 && path[i] != '/'; i--);

  return path + i + 1;
}



//  
//
//

void
create_cripta(char * path)
{
  struct directory * dir = new_dir(path);
  if (dir == NULL) return;
  const char * file_name_suffix = "_CRIPTA";
  //create new file
  char * new_file_name = malloc(strlen(path) + strlen(file_name_suffix) + 1);
  sprintf(new_file_name, "%s%s", path, file_name_suffix);

  FILE * new_f = fopen(path_leaf(new_file_name), "w+");
  if (new_f == NULL)
    {
      error("Unable to open new file");
      return;
    }

  //initiate srand for dealing with encryption (so that it doesn't suck)
  srand(time(NULL));

  create_cripta_with_father(dir, new_f);

  fclose(new_f);
  directory_free(dir);
}


//
//  create_cripta_with_father:
//     Creating a cripta, will start by going down each directory, until it reaches
//     the directories' roots. Writing all the files and then the directory meta at the      
//     initial position of the file.
//  
//
//  dir_index - index for replacing its directory info in the father meta (should start at 0)
//  father_meta - should be NULL, not necessary for root directory

void
create_cripta_with_father(struct directory * dir, FILE * file)
{
  int dir_meta_position = ftell(file);
  int meta_size;
  char * my_meta = create_dir_meta(dir, &meta_size);

  fseek(file, meta_size, SEEK_CUR);  //reserve space for when writing meta

  int dirNumber = 0;
  ListNode * node = dir->directories->head;
  while (node != NULL)
    { //Recursively write each directories information into the same file
    add_dir_offset_meta(my_meta, ftell(file), dirNumber++);
    create_cripta_with_father((struct directory *)node->data, file);
    node = node->next;
    }

  node = dir->files->head;
  int fileNumber = 0, file_size;
  unsigned char * file_array, * hash;
  while (node != NULL)
    {
    add_file_offset_meta(my_meta, ftell(file), fileNumber++);
    file_array = write_cripta_file((char*)node->data, &file_size);

    fwrite(file_array, sizeof(unsigned char), file_size, file);
      
    free(file_array);
    node = node->next;
    }
   printf("I am at: %d meta position is: %d\n", (int)ftell(file), dir_meta_position);
  //after all files and dirs seen, write the directory meta fully updated
  fseek(file, dir_meta_position, SEEK_SET);
  fwrite(my_meta, sizeof(char), meta_size, file);
  free(my_meta);
  //always append at the end of the file 
  fseek(file, 0, SEEK_END);

}

//
//  write_cripta_file - returns the information about a file needed for it to be stored
//    A directory meta will have the offset to a file
//    In the file's header there will be:
//      1) file name length - 2 bytes
//      2) file name - (file name length)xbytes
//      3) md5 hash
//      4) length of the file
//      4) content of the file
//
//  @param  path  that points to the file
//  @param  size  for passing the final array size length by parameter
//
//  @return       a byte array with the file information 
//                with length inserted into parameter size
//
unsigned char *
write_cripta_file(char * path, int * size)
{
    
    int filename_size = strlen(path);

    int file_size;
    char * file_content = read_full_file(path, &file_size);
    unsigned char * hash = get_hash(file_content, file_size);
    
    unsigned char * b_file_name_length = int_to_bytes(filename_size, FILE_NAME_LENGTH);
    unsigned char * b_file_size_length = int_to_bytes(file_size, FILE_SIZE_LENGTH);

    *size = FILE_NAME_LENGTH + filename_size 
            + MD5_SIZE + FILE_SIZE_LENGTH + file_size;

    unsigned char * file_array = malloc(*size);
    //length of the filename
    memcpy(file_array, b_file_name_length, FILE_NAME_LENGTH);
    //filename
    memcpy(file_array + FILE_NAME_LENGTH, path, filename_size);
    //md5 hash
    memcpy(file_array + FILE_NAME_LENGTH + filename_size, hash, MD5_SIZE);
    //length of the file
    memcpy(file_array + FILE_NAME_LENGTH + filename_size + MD5_SIZE,
           b_file_size_length, FILE_SIZE_LENGTH);
    //content of the file
    memcpy(file_array + FILE_NAME_LENGTH + filename_size + MD5_SIZE + FILE_SIZE_LENGTH,
            file_content, file_size);

    free(b_file_name_length);
    free(b_file_size_length);
    free(file_content);
    free(hash);
    return file_array;

}

void
cripta_file_free(cripta_file * cf)
{
  free(cf->name);
  free(cf->hash);
  free(cf);
}

//
//  read_cripta_file - takes a cripta file that points to a file and returns a  
//       structure with all the information on the file. After readind that data from
//       the cripta it points to the end of the initial offset. 
//        (pointing to another file offset in the meta or the end of the meta)
//
cripta_file *
read_cripta_file(FILE * cripta)
{

  unsigned char b_file_offset[FILE_OFFSET_LENGTH];
  fread(b_file_offset, sizeof(unsigned char), FILE_OFFSET_LENGTH, cripta);
  int file_offset = bytes_to_int(b_file_offset, FILE_OFFSET_LENGTH);
  int next_pos_in_cripta = ftell(cripta);
  fseek(cripta, file_offset, SEEK_SET);


  unsigned char b_file_name_length[FILE_NAME_LENGTH];
  fread(b_file_name_length, sizeof(unsigned char), FILE_NAME_LENGTH, cripta);

  int file_size = bytes_to_int(b_file_name_length, FILE_NAME_LENGTH);

  char * filename = malloc(file_size + 1);
  fread(filename, sizeof(char), file_size, cripta);
  filename[file_size] = '\0';

  unsigned char * hash = malloc(MD5_SIZE);
  fread(hash, sizeof(unsigned char), MD5_SIZE, cripta);
  
  unsigned char b_file_size_length[FILE_SIZE_LENGTH];
  fread(b_file_size_length, sizeof(unsigned char), FILE_SIZE_LENGTH, cripta);

  cripta_file * c_file = malloc(sizeof(cripta_file));
  c_file->name = filename;
  c_file->hash = hash;
  c_file->content_offset = ftell(cripta);
  c_file->content_size = bytes_to_int(b_file_size_length, FILE_SIZE_LENGTH);
  fseek(cripta, next_pos_in_cripta, SEEK_SET);

  return c_file;

}

//
//  create_cripta_file_content - creates a new file locally taken from the CRIPTA file
//        Even it the hash is wrong, the file is created, but the user should be warned.
//
//  @returns        0 or 1 if unsucceful or successul 
//  
int
create_cripta_file_content(FILE * cripta, cripta_file * file)
{
  fseek(cripta, file->content_offset, SEEK_SET);
  unsigned char * content = malloc(file->content_size);
  fread(content, sizeof(unsigned char), file->content_size, cripta);
  
  FILE * new_file = fopen(path_leaf(file->name), "w+");
  fwrite(content, sizeof(unsigned char), file->content_size, new_file);

  return 1;//validate(content, file->content_size, file->hash);
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
//  @param    dir   the directory correspondent to the meta to create
//  @return         the meta as a string
//
//
//  Restrictions:   max directory name size = 2^(16)
//  (single meta)   max files/directories = 2 ^ (16)
//


char *
create_dir_meta(struct directory * dir, int * meta_size) 
{
  const int path_name_length = strlen(dir->name);
  *meta_size = PATH_SIZE_LENGTH + path_name_length
          + DIR_COUNT_LENGTH + (DIR_OFFSET_LENGTH * dir->directories->length)  
          + FILE_COUNT_LENGTH + (FILE_OFFSET_LENGTH * dir->files->length)
          + 1;

  int meta_offset = 0;

  char * meta = malloc(*meta_size);
  unsigned char * b_path_name_length = int_to_bytes(path_name_length, PATH_SIZE_LENGTH);

  memcpy(meta + meta_offset, b_path_name_length, PATH_SIZE_LENGTH);
  free(b_path_name_length);

  meta_offset += PATH_SIZE_LENGTH;
  memcpy(meta + meta_offset, dir->name, path_name_length);

  meta_offset += path_name_length;

  unsigned char * b_dir_count_length = int_to_bytes(dir->directories->length, DIR_COUNT_LENGTH);

  memcpy(meta + meta_offset, b_dir_count_length, DIR_COUNT_LENGTH);
  free(b_dir_count_length);
  
  meta_offset += DIR_COUNT_LENGTH;
  int i;
  for(i = 0; i < dir->directories->length; i++)
    { //reserve 4 bytes
      memcpy(meta + meta_offset,DIR_OFFSET_PADDING, DIR_OFFSET_LENGTH);
      meta_offset += DIR_OFFSET_LENGTH;
    }

  unsigned char * b_file_count_length = int_to_bytes(dir->files->length, FILE_COUNT_LENGTH);
  memcpy(meta + meta_offset, b_file_count_length, FILE_COUNT_LENGTH);
  free(b_file_count_length);

  meta_offset += FILE_COUNT_LENGTH;
  int j;
  for(j = 0; j < dir->files->length; j++)
    { //reserve 4 bytes
      memcpy(meta + meta_offset, FILE_OFFSET_PADDING, FILE_OFFSET_LENGTH);
      meta_offset += FILE_OFFSET_LENGTH;
    }

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
//
void
add_file_offset_meta(char * meta, int file_offset, int pos)
{
  int path_size = bytes_to_int(meta, PATH_SIZE_LENGTH);
  meta += PATH_SIZE_LENGTH + path_size;

  int dir_count = bytes_to_int(meta, DIR_COUNT_LENGTH);
  meta += DIR_COUNT_LENGTH + (DIR_OFFSET_LENGTH*dir_count)
          + FILE_COUNT_LENGTH + (FILE_OFFSET_LENGTH * pos);

  unsigned char * b_file_offset = int_to_bytes(file_offset, FILE_OFFSET_LENGTH);
  memcpy(meta, b_file_offset, FILE_OFFSET_LENGTH);
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
void
add_dir_offset_meta(char * meta, int dir_offset, int pos)
{
  int path_size =  bytes_to_int(meta,PATH_SIZE_LENGTH);
  meta += PATH_SIZE_LENGTH + path_size 
          + DIR_COUNT_LENGTH + (DIR_OFFSET_LENGTH * pos);

  unsigned char * b_dir_offset = int_to_bytes(dir_offset, DIR_OFFSET_LENGTH);
  memcpy(meta, b_dir_offset, DIR_OFFSET_LENGTH);
  free(b_dir_offset);
}


unsigned char *
int_to_bytes(int integer, int num_of_bytes)
{
  if (num_of_bytes <= 0) 
    return NULL;

  unsigned char * bytes_array =(unsigned char *)malloc(num_of_bytes);
  int i, shift = (num_of_bytes - 1) * BYTE;
  for (i = 0; i< num_of_bytes; i++)
    {
      bytes_array[i] = integer >> shift;
      shift -= BYTE;
    }
  return bytes_array;
}

int
bytes_to_int(unsigned char * bytes, int num_of_bytes)
{
  if (num_of_bytes <= 0) 
    return 0;

  int integer = 0;
  int i, shift = (num_of_bytes - 1) * BYTE;
  for (i = 0; i< num_of_bytes; i++)
    {
      integer |= (bytes[i] << shift);
      shift -= BYTE;
    }
  return integer;
}



//
//  Receives the directory and the name of the path of the dir
//
int dir_cmp(const void * dir, const void * name)
{
  return strcmp(((struct directory*)dir)->name, (char*)name);
}
//
//
int file_cmp(const void * file, const void * name)
{
  return strcmp(((struct cripta_file*)file)->name, (char*)name);
}



// 
// The size of the file is the difference between the offset of the next file 
// and the current file (without the hash (-32bytes)),
//
struct directory * 
read_cripta_dir(FILE * cripta)
{
  
  unsigned char b_path_length[PATH_SIZE_LENGTH];

  fread(&b_path_length, PATH_SIZE_LENGTH, sizeof(unsigned char), cripta);
  int path_name_length = bytes_to_int(b_path_length, PATH_SIZE_LENGTH);

  char * root_path_name = malloc(path_name_length + 1);
  fread(root_path_name, path_name_length, sizeof(unsigned char), cripta);
  root_path_name[path_name_length] = '\0';

  unsigned char b_num_dirs[DIR_COUNT_LENGTH], b_offset_dir[DIR_OFFSET_LENGTH];
  fread(b_num_dirs, DIR_COUNT_LENGTH, sizeof(unsigned char), cripta);
  int num_dirs = bytes_to_int(b_num_dirs, DIR_COUNT_LENGTH);


  List * dir_list = list_new(NULL);
  int i;
  int checkpoint_index;
  for (i=0; i < num_dirs; i++)
    {
      //call read_cripta_dir() recursovely for each directory offset    
      fread(b_offset_dir, DIR_OFFSET_LENGTH, sizeof(unsigned char), cripta);
      checkpoint_index = ftell(cripta);
      fseek(cripta, bytes_to_int(b_offset_dir, DIR_OFFSET_LENGTH), SEEK_SET);
      list_add(dir_list, read_cripta_dir(cripta));
      fseek(cripta, checkpoint_index, SEEK_SET);
    }


  unsigned char b_num_files[FILE_COUNT_LENGTH];
  fread(b_num_files, FILE_COUNT_LENGTH, sizeof(unsigned char), cripta);
  int num_files = bytes_to_int(b_num_files, FILE_COUNT_LENGTH);

  int j;  
  List * file_list = list_new(file_cmp);
  for (i=0; i < num_files; i++)
    {
      list_add(file_list, read_cripta_file(cripta));
    }

  struct directory * crip_dir = malloc(sizeof(struct directory));
  crip_dir->name = root_path_name;
  crip_dir->directories = dir_list;
  crip_dir->files = file_list;

  return crip_dir;
}

void print_dirs(struct directory * dir)
{
  ListNode * node = dir->directories->head;
  int counter = 1;
  while (node!= NULL)
  {
    printf("%d) %s\n", counter++, ((struct directory *)node->data)->name);
    node = node->next;
  }
  node = dir->files->head;
  while (node != NULL)
  {
    printf("%d) %s\n", counter++,((cripta_file*)node->data)->name);
    node = node->next;
  }
  puts(">back\n>exit");
}

void
help()
{
  puts("cripta -c <path>\t\t encrypt a file into a CRIPTA");
  puts("cripta -d <file>\t\t starts read CRIPTA file mode");
}


void
_cmd(char * cripta_name)
{


  FILE * cripta = fopen(cripta_name, "r");
  if (cripta == NULL){
    error("FILE NOT FOUND");
    return;
  }

  List * stack = list_new(NULL); 

  struct directory * dirs = read_cripta_dir(cripta);
  struct directory * dir_free = dirs; //used for cleaning the entire structure
  char buffer[1024];


  while (1)
    {
    print_dirs(dirs);
    printf(">> ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strlen(buffer)-1] = '\0';

    if (strcmp(buffer, "back") == 0)
      {
      if (stack->head != NULL)
        dirs = (struct directory *)list_pop(stack);
      else 
        puts("Already at root");
      goto finish_cmd_round;
      }
    else if (strcmp(buffer, "exit") == 0)
      {
      break;
      }

    int option = atoi(buffer);
    int counter = 1;
    ListNode * node = dirs->directories->head;
    while (node!=NULL)
      {
      if (counter == option)
        {
        list_add(stack, dirs);
        dirs = (struct directory *)node->data; 
        goto finish_cmd_round;
        }
      counter++;
      node = node->next;
      }
    node = dirs->files->head;
    while (node != NULL)
      {

      if (counter == option)
        {
        printf("Creating file: %s\n", ((cripta_file *)node->data)->name);
        create_cripta_file_content(cripta, (cripta_file *)node->data);
        goto finish_cmd_round;
        }
      counter++;
      node = node->next;
      }

      puts("Invalid directory.");
finish_cmd_round: //Not a proud moment for me
;
    }
    list_free(stack);
    directory_free_with_files(dir_free);
    fclose(cripta);
}


int
main(int argc, char * argv[])
{
  char * dir_name;
  int freeable = 0;
  if (argc < 2)
    {
    help();
    return EXIT_FAILURE;
    }

  int i;
  char * flag;
  for (i = 1; i + 1 < argc; i+=2)
    { 
    flag = argv[i];
    if (strlen(flag) != 2 || flag[0] != '-')
      {
      help();
      return EXIT_SUCCESS;
      }

    dir_name = malloc(strlen(argv[i+1]) + 1);
    strcpy(dir_name, argv[i+1]);

    switch (flag[1])
      {
      case 'c':
        create_cripta(dir_name);
        break;

      case 'd':
        _cmd(dir_name);
        break;

      default:
        help();
      }

    free(dir_name);
    }
  
return EXIT_SUCCESS;

}