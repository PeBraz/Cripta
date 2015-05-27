#include "list.h"

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
  list->length--;
  return data;
}

void
list_free(List * list)
{
  ListNode * node = list->head,* prev_node = NULL;
  while (node != NULL)
  {
    prev_node = node;
    node = node->next;
    free(prev_node->data);
    free(prev_node);
  }
  free(list);
}

int int_cmp(const void * a, const void * b)
{
  return *(int*)a - *(int*)b;
}
