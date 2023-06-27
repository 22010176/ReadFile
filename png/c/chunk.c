#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "../../utils/File.h"

typedef struct _chunk Chunk;
struct _chunk {
  int size, pos;
  char name[100], crf[5], * data;
  char* (*SetData)(Chunk* obj, char*, int);
  char* (*SetCrf)(Chunk* obj, char*);
  void (*Print)(Chunk*, int);
  void (*Free)(Chunk*);
  void (*FPrint)(Chunk*, FILE*, int);
};
void _ChPrint(Chunk* obj, int prda) {
  Chunk _th = *obj;
  printf("_____________________________________________\n");
  printf("name: %-10ssize: %-10dpos: %-10d\n", _th.name, _th.size, _th.pos);
  printf("Crf: ");
  for (int i = 0; i < 4;i++) printf("%-3x", GetReal(_th.crf[i], 8));
  printf("\n");
  if (!prda || !obj->size) return;
  printf("Data:\n");
  for (int i = 0; i < obj->size;i++) printf("%*x%c", -3, GetReal(obj->data[i], 8), AddIndent(i + 1, 16));
  printf("\n\n");
}

char* _ChSetData(Chunk* obj, char* dat, int l) {
  obj->size = l;
  obj->data = (char*)memcpy(realloc(obj->data, l + 1), dat, l);
  obj->data[l] = '\0';
  return obj->data;
}
void _ChFPrint(Chunk* obj, FILE* f, int prda) {
  Chunk _th = *obj;
  fprintf(f, "_____________________________________________\n");
  fprintf(f, "name: %-10ssize: %-10dpos: %-10d\n", _th.name, _th.size, _th.pos);
  fprintf(f, "Crf: ");
  for (int i = 0; i < 4;i++) fprintf(f, "%-3x", GetReal(_th.crf[i], 8));
  fprintf(f, "\n");
  if (!prda || !obj->size) return;
  fprintf(f, "Data:\n");
  for (int i = 0; i < obj->size;i++) fprintf(f, "%*x%c", -3, GetReal(obj->data[i], 8), AddIndent(i + 1, 16));
  fprintf(f, "\n\n");
}
char* _ChSetCrf(Chunk* obj, char* crf) { return strncpy(obj->crf, crf, 4); }
void __Chunk(Chunk* obj) {
  free(obj->data);
  free(obj);
}
Chunk* _Chunk(int size, int pos, char* name) {
  Chunk* _ch = (Chunk*)malloc(sizeof(Chunk));
  strcpy(_ch->name, name);
  _ch->data = (char*)calloc(1, 1);
  _ch->size = size, _ch->pos = pos;
  _ch->Print = _ChPrint, _ch->Free = __Chunk;
  _ch->SetData = _ChSetData, _ch->SetCrf = _ChSetCrf;
  _ch->FPrint = _ChFPrint;
  return _ch;
}
#if __INCLUDE_LEVEL__ == 0
int main() {
  char a[] = "C:/Users/ducmi/Downloads/New folder/data/png/sample2.png";
  // FILE* f = fopen(a, "rb");
  Chunk* _a = _Chunk(10, 5, (char*)"Hello");
  _a->SetData(_a, (char*)"abcd6669", 30);
  _a->SetCrf(_a, (char*)"1234");
  _a->SetData(_a, (char*)"012345678911234567890", 141);
  _a->Print(_a, 1);
  _a->Free(_a);
}
#else
#pragma once
#endif
