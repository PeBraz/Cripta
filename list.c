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
  return &(List){.head=NULL, .cmp=cmp};
}

void
list_add(List * list, void * ele)
{
  list->head = &(ListNode){ .data=ele, .next=list->head};
}
void *
list_get(List * list, void * ele)
{
  ListNode * node = list->head;
  while (node != NULL && !list->cmp(ele, node))
      node=node->next;

  return (node != NULL) ? node->data : NULL;
}

void *
list_remove(List * list, void * ele)
{

  ListNode * node = list->head, * prev_node = NULL;
  while (node != NULL && !list->cmp(ele, node))
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

void *
integer(int i)
{
  int * pi = malloc(sizeof(int));
  *pi = i;
  return pi;
}

int
main()
{

  List * l = list_new(int_cmp);
  list_add(l, integer(4));
  list_get(l,integer(3));
  list_get(l,integer(4));
  list_get(l,integer(4));
  list_remove(l,integer(3));
  list_remove(l,integer(4));
  list_remove(l,integer(4));

  list_add(l, integer(3));

  return EXIT_SUCCESS;
}
