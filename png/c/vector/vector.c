#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "math.h"

#define VECTOR_INIT_SIZE 4
#define NOVALUE (void*)-1
#define TEMP (void*)-2
#define END (void*)-3

typedef struct _vector Vector;
struct _vector {
  void** _data;
  int _len, _init_len, _max_index;
  int  _size, _vsize;
  int* _free_space, _cur_free;

  int (*Length)(Vector*); // get Real Length

  int (*Add)(Vector*, void*); // Add data to the free space
  void* (*_Resize)(Vector*, int); // Resize Vector when the size of data is full

  void* (*At)(Vector*, int); // Get the index
  void* (*Find)(Vector*, int (*)(void*, void*)); // Find the data match the funciton condition

  void (*Remove)(Vector*, int); // remove element and add free space to free_space
  void (*FindIndex)(int*); // 
  void (*Free)(Vector*);
};

void* _VAt(Vector* obj, int i) {
  if (i >= obj->_max_index) return NOVALUE;
  return obj->_data[i];
}
int _VAdd(Vector* obj, void* item) {
  if (obj->_max_index == obj->_vsize) obj->_data = (void**)obj->_Resize(obj, obj->_vsize);
  int i = obj->_max_index++;
  obj->_data[i] = item;

  obj->_init_len = 0;
  return i;
}
int _VLength(Vector* obj) {
  if (obj->_init_len) return obj->_len;
  for (int i = 0; obj->_data[i] != END; obj->_len = ++i);
  obj->_init_len = 1;
  return obj->_len;
}
void _VRemove(Vector* obj, int index) {
  if (!obj->_cur_free) obj->_free_space[obj->_cur_free++] = index;
  obj->_data[index] = NOVALUE;
}
void* _VFind(Vector* obj, int (*f)(void*, void*)) {}
void __Vector(Vector* obj) {}
void* __VResize(Vector* obj, int len) {
  return NOVALUE;
}
Vector* _Vector() {
  Vector* res = (Vector*)malloc(sizeof(Vector));
  res->_vsize = VECTOR_INIT_SIZE;
  res->_size = sizeof(void*) * res->_vsize;
  res->_data = (void**)malloc(res->_size);
  res->_cur_free = res->_init_len = res->_max_index = 0;
  res->_free_space = (int*)malloc(100 * sizeof(int));

  res->Add = _VAdd, res->Remove = _VRemove, res->Find = _VFind;
  res->Free = __Vector, res->At = _VAt, res->Length = _VLength;
  res->_Resize = __VResize;
  return res;
}

#if __INCLUDE_LEVEL__ == 0
int main() {
  printf("Hello");
}
#else
#pragma once
#endif