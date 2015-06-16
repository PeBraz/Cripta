#include "list.h"


void _list_free(List*,int);

List *
list_new(int (*cmp)(const void*, const void*))
{
  List * list = malloc(sizeof(List));
  list->head = NULL;
  list->cmp = cmp;
  list->length = 0;
  return list;
}

void
list_add(List * list, void * ele)
{
  ListNode * node = malloc(sizeof(ListNode));
  node->data = ele;
  node->next = list->head;
  list->head = node;
  list->length++;
}
void *
list_get(List * list, void * ele)
{
  ListNode * node = list->head;
  while (node != NULL && list->cmp(node->data, ele))
      node=node->next;

  return (node != NULL) ? node->data : NULL;
}

void *
list_pop(List * list)
{
    return list_remove(list, list->head->data);
}

void *
list_remove(List * list, void * ele)
{

  ListNode * node = list->head, * prev_node = NULL;

  while (node != NULL && list->cmp != NULL && list->cmp(node->data, ele))
    {
      prev_node = node;
      node=node->next;
    }
  if (node == NULL)
    return NULL;  
  else if (prev_node == NULL)
   {
     list->head = node->next;
   }
  else {
    prev_node->next = node->next;
  }
  void * data = node->data;
  free(node);
  list->length--;
  return data;
}

void list_free(List * list)
{
  _list_free(list, 0);
}


void list_full_free(List * list)
{
  _list_free(list, 1);
}

//
//  _list_free - Deallocates all memory associated with the list.
//      Can also choose to keep the data blocks in memory, making the user responsible for cleaning 
//      those blocks;  
//
//  @param  list  created by list_new() to be destroyed
//  @param  do_full_free flag for the user to choose what kind of cleanup he wants.
//  

void
_list_free(List * list, int do_full_free)
{
  ListNode * node = list->head,* prev_node = NULL;
  while (node != NULL)
  {
    prev_node = node;
    node = node->next;

    if (do_full_free) 
      free(prev_node->data);

    free(prev_node);
  }
  free(list);
}

int int_cmp(const void * a, const void * b)
{
  return *(int*)a - *(int*)b;
}
