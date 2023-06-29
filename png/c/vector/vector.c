#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "math.h"

#define VECTOR_INIT_SIZE 4
#define VNULL (void*)-1
#define TEMP (void*)-2
#define END (void*)-3

typedef struct _vector Vector;
struct _vector {
  void** _data;
  int _len, _size;

  void* (*At)(Vector*, int);
  int (*Length)(Vector*);
  void (*Insert)(Vector*);
  void (*InsertAt)(Vector*, void*);
  void (*Remove)(Vector*, void*, int);
  void (*Free)(Vector*);
  void (*_Shift)(Vector*, int, int);
  void (*_Resize)(Vector*);
};

void* _VAt(Vector* obj, int i) {
  if (i < obj->Length(obj)) return VNULL;
  return obj->_data[i];
}
int _VLength(Vector* obj) {}
void _VInsert(Vector* obj, void* item) {}
void _VInsertAt(Vector* obj, void* item, int index) {}
void _VRemove(Vector*, int i) {};
void __Vector(Vector* obj) {}

Vector* _Vector() {
  Vector* _data = (Vector*)malloc(sizeof(Vector));
  _data->_size = VECTOR_INIT_SIZE;
  _data->_data = (void**)malloc((_data->_size + 1) * sizeof(void*));
  for (int i = 0; i < _data->_size;i++) _data->_data[i] = TEMP;
  _data->_data[_data->_size] = END;

  _data->At = _VAt, _data->Length = _VLength, _data->Insert = _VInsert;
  _data->Remove = _VRemove, _data->Free = __Vector;
  return _data;
}

#if __INCLUDE_LEVEL__ == 0
int main() {
  printf("Hello");
}
#else
#pragma once
#endif