#include <stdlib.h>

typedef struct _node {
  int num, _count, _len;
  void** data;
  struct _node* next;
  void (*insertNode)(int, struct _node*), (*deleteNode)(int), (*PrintList)();
  int (*IndexOf)(struct _node*);
  struct _node* (*at)(int);
} Vector;

void _insertNode(int, struct _node*) {}
void _deleteNode(int index) {}
void _printList() {}
struct _node* _at(int index) {}
int _IndexOf(struct _node*) {}
Vector* _CreateNode() {
  Vector* res = malloc(sizeof(Vector));
  res->insertNode = _insertNode, res->deleteNode = _deleteNode, res->PrintList = _printList;
  res->at = _at, res->IndexOf = _IndexOf;
  return res;
}
#if __INCLUDE_LEVEL__ == 0
int main() {}
#else
#pragma once
#endif