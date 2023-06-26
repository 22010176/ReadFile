#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "stdlib.h"
#include "../Data.h"

#include "../../utils/File.h"
#include "../../utils/Number.h"
#include "../../utils/header.h"

typedef struct _ch Chunk;
struct _ch {
  Data** data;
  int len, pos, dat_len, crf[4];
  char name[5];
  void (*AddData)(Chunk*, Data*);
  Data* (*GetDataAt)(Chunk*, int);
  Data* (*GetDataByName) (Chunk*, char*);
  void (*DelData)(Chunk*, int);
  void (*Free)(Chunk*);
  char* (*GetStr)();
};
void _AddData(Chunk* self, Data* item) {
  self->data = (Data**)realloc(self->data, (self->dat_len + 1) * sizeof(Data*));
  self->data[self->dat_len] = item;
  self->data[++self->dat_len] = (Data*)-1;
}
Data* _GetDataAt(Chunk* self, int x) {
  if (x >= self->dat_len) return (Data*)-1;
  return self->data[x];
}
Data* _GetDataByName(Chunk* self, char* name) {
  Data** dat = self->data;
  for (int i = 0; i < self->dat_len;i++)
    if (!!strcmp(dat[i]->name, name)) return dat[i];
  return (Data*)-1;
}
void _DelData(Chunk* self, int _i) {
  Data* dat[self->dat_len + 1];
  int j = 0;
  for (int i = 0; i < self->dat_len;i++) if (i != _i)dat[j++] = self->data[i];
  dat[j++] = (Data*)-1;
  self->dat_len--;
  memcpy(self->data, dat, j);
  self->data = (Data**)realloc(self->data, j * sizeof(Data*));
}
void _FreeC(Chunk* self) {
  int len = GetLen((void**)self->data);
  for (int i = 0; i < len;i++) free(self->data[i]);
  free(self->data);
  free(self);
}
Chunk* _Chunk() {
  Chunk* dat = (Chunk*)malloc(sizeof(Chunk));
  dat->dat_len = 4;
  dat->data = (Data**)malloc((dat->dat_len + 1) * sizeof(Data*));
  dat->data[dat->dat_len] = (Data*)-1;
  dat->AddData = _AddData;
  dat->GetDataAt = _GetDataAt;
  dat->GetDataByName = _GetDataByName;
  dat->DelData = _DelData;
  dat->Free = _FreeC;
  return dat;
}
#if __INCLUDE_LEVEL__ == 0
int main() {
  char path[100];
  strcat(path, png);
  strcat(path, "sample.png");
  FILE* f = fopen(path, "rb");
  int len = 4;
  Chunk* a = _Chunk(f, 8, (Data**)-1);

}
#else
#pragma once
#endif