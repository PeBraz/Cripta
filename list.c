#include <stdio.h>
#include <stdlib.h>

typedef struct list_node
{
  void * data;
  struct list_node * next;
} ListNode;

typedef struct List
{
  ListNode * head;
  int (*cmp)(const void *,const void*);
} List;


List *
list_new(int (*cmp)(const void*, const void*))
{
  List * list = malloc(sizeof(List));
  list->head = NULL;
  list->cmp = cmp;
  return list;
}

void
list_add(List * list, void * ele)
{
  ListNode * node = malloc(sizeof(ListNode));
  node->data = ele;
  node->next = list->head;
  list->head = node;
}
void *
list_get(List * list, void * ele)
{
  ListNode * node = list->head;
  while (node != NULL && list->cmp(ele, node->data))
      node=node->next;

  return (node != NULL) ? node->data : NULL;
}

void *
list_remove(List * list, void * ele)
{

  ListNode * node = list->head, * prev_node = NULL;

  while (node != NULL && list->cmp(ele, node->data))
    {
      prev_node = node;
      node=node->next;
    }
  if (node == NULL)
    return NULL;
  else if (prev_node == NULL)
   {
     list->head = NULL;
   }
  else {
    prev_node->next = node->next;
  }
  void * data = node->data;
  free(node);
  return data;
}

int int_cmp(const void * a, const void * b)
{
  return *(int*)a - *(int*)b;
}
