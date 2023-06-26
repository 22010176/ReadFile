#include "stdio.h"
#include "stdlib.h"
#include "conio.h"
#include "math.h"
#include "string.h"

#include "../../utils/File.h"
#include "../../utils/header.h"

typedef struct _da Data;
struct _da {
  char* data, name[101], * formated;
  int len, pos, _is_formated;
  int (*GetInt)(Data*);
  char* (*PrintData)(Data*);
  char* (*GetStr)(Data*);
  void (*SetData)(Data*, char*, int);
  void (*Free)(Data*);
};
int _GetInt(Data* data) { return GetIntData(data->data, data->len); }
char* _GetStr(Data* data) { return GetStrData(data->data, data->len); }
char* _PrintData(Data* data) {
  char a[data->len * 3], temp[10];
  a[0] = '\0';
  if (data->_is_formated) return data->formated;
  for (int i = 0; i < data->len;i++) {
    sprintf(temp, "%d ", GetReal(data->data[i], 8));
    strcat(a, temp);
  }
  data->formated = (char*)memcpy(malloc(strlen(a) + 1), a, strlen(a) + 1);
  data->_is_formated = 1;
  return data->formated;
}
void _SetData(Data* self, char* data, int len) {
  memcpy(self->data, data, len + 1);
  self->data[len] = '\0';
  self->data = (char*)realloc((void*)self->data, len + 1);
}
void _FreeD(Data* data) {
  free(data->data);
  free(data->formated);
  free(data);
}
Data* _Data(FILE* f, int pos, int len, char* name) {
  char* _dat = ReadData(f, pos, len);
  Data* res = (Data*)malloc(sizeof(Data));
  strcpy(res->name, name);
  res->_is_formated = 0;
  res->pos = pos, res->len = len, res->data = _dat;
  res->GetStr = _GetStr, res->GetInt = _GetInt;
  res->PrintData = _PrintData, res->Free = _FreeD;
  res->SetData = _SetData;
  return res;
}
#if __INCLUDE_LEVEL__ == 0
int main() {
  char path[100];
  strcat(path, png);
  strcat(path, "sample.png");
  FILE* f = fopen(path, "rb");
  Data* a = _Data(f, 40, 10, (char*)"header");
  printf(a->PrintData(a));
  fclose(f);
  a->Free(a);
}
#else
#pragma once
#endif
