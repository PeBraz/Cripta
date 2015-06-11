#include <stdio.h>
#include <stdlib.h>

#ifndef _LIST_H_
#define _LIST_H_
typedef struct list_node
{
  void * data;
  struct list_node * next;
} ListNode;

typedef struct List
{
  ListNode * head;
  int length;
  int (*cmp)(const void *,const void*);
} List;


List * list_new(int (*cmp)(const void*, const void*));

void list_add(List * list, void * ele);

void * list_get(List * list, void * ele);

void * list_remove(List * list, void * ele);

void list_free(List * list);

void * list_pop(List * list);

void list_full_free(List * list);

int int_cmp(const void * a, const void * b);


#endif
