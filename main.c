#include <stdio.h>
#include <string.h>
#include <math.h>
#include "stdlib.h"
#include "zlib/zlib.h"

typedef struct { char* data;  int len; } Data;
typedef struct {
  int size;
  char name[5], crf[5], * data;
} Chunk;
int GetReal(int x, int p) { // visual only
  if (x >= 0) return x;
  return (int)pow(2, p) + x;
}
int GetINT(char* x, int l) {
  int z = 0;
  for (int i = 0; i < l;i++) z += GetReal(x[l - i - 1], 8) * (int)pow(16, i * 2);
  return z;
}
int GetColorByte(int ct) {
  switch (ct) {
  case 0:
  case 3: return 1;
  case 2: return 3;
  case 4: return 2;
  case 6: return 4;
  default: return 0;
  }
}
int CalcDataLen(int w, int h, int ct) { return  w * h * GetColorByte(ct) + h; }

char* ReadData(FILE* t, int pos, int len) {
  fseek(t, pos, SEEK_SET);
  char* a = (char*)malloc(len + 1);
  fread(a, 1, len, t); a[len] = '\0';
  return a;
}
int GetWidth(FILE* t) { // 16
  char* w = ReadData(t, 16, 4);
  int width = GetINT(w, 4);
  free(w);
  return width;
}
int GetHeight(FILE* t) { // 20
  char* w = ReadData(t, 20, 4);
  int h = GetINT(w, 4);
  free(w);
  return h;
}
int GetBitDepth(FILE* t) { // 24
  char* w = ReadData(t, 24, 1);
  int d = GetINT(w, 1);
  free(w);
  return d;
}

int GetColorType(FILE* t) { // 25
  char* w = ReadData(t, 25, 1);
  int d = GetINT(w, 1);
  free(w);
  return d;
}
int GetCompress(FILE* t) { // 26
  char* w = ReadData(t, 26, 1);
  int d = GetINT(w, 1);
  free(w); return d;
}
int GetFilterMethod(FILE* t) { // 27
  char* w = ReadData(t, 27, 1);
  int d = GetINT(w, 1);
  free(w); return d;
}
int GetInterlac(FILE* t) { // 28
  char* w = ReadData(t, 28, 1);
  int d = GetINT(w, 1);
  free(w); return d;
}
Chunk* _Chunk(FILE* t, int pos) {
  Chunk* res = (Chunk*)malloc(sizeof(Chunk));
  char* s = ReadData(t, pos, 4);
  res->size = GetINT(s, 4);
  free(s);
  s = ReadData(t, pos + 4, 4);
  strcpy(res->name, s);
  free(s);

  return res;
}
Data* GetData(FILE* t) {
  Data* x = (Data*)malloc(sizeof(Data));
  x->len = 0, x->data = (char*)calloc(x->len + 1, 1);
  fseek(t, 8, SEEK_SET);
  int cur = ftell(t), i = 0;
  char* _t;
  while (getc(t) != EOF) {
    _t = ReadData(t, cur, 4);
    int size = GetINT(_t, 4);
    free(_t);
    cur = ftell(t);
    _t = ReadData(t, cur, 4);
    cur = ftell(t);

    if (!strcmp(_t, "IEND")) {
      free(_t);
      break;
    }
    if (strcmp(_t, "IDAT")) {
      free(_t);
      cur += size + 4;
      continue;
    }

    char* data = ReadData(t, cur, size);
    do { x->data = realloc(data, x->len + size + 1); } while (!x->data);
    memcpy(x->data + x->len, data, size);
    x->len += size;
    cur += 4 + size;
    free(_t);
  }
  x->data[x->len] = '\0';
  return x;
}
Data* Decompress(Data* x, FILE* t) {
  Data* res = (Data*)malloc(sizeof(Data));
  int  width = GetWidth(t), height = GetHeight(t), ct = GetColorType(t);
  res->len = CalcDataLen(width, height, ct);
  do { res->data = (char*)calloc(res->len + 1, 1); } while (!res->data);

  z_stream a;
  a.zalloc = Z_NULL;
  a.zfree = Z_NULL;
  a.opaque = Z_NULL;

  a.avail_in = (uInt)(x->len + 1);
  a.next_in = (Bytef*)x->data;
  a.avail_out = (uInt)(res->len + 1);
  a.next_out = (Bytef*)res->data;

  inflateInit(&a);
  inflate(&a, Z_NO_FLUSH);
  inflateEnd(&a);

  res->data[res->len] = '\0';
  return res;
}

int None(int a, int b, int c, int x) { return x; }
int Sub(int a, int b, int c, int x) { return (x + a) % 256; }
int Up(int a, int b, int c, int x) { return (x + b) % 256; }
int Average(int a, int b, int c, int x) { return (x + (int)floor((a + b) / 2)) % 256; }
int Paeth(int a, int b, int c, int x) {
  int p = a + b - c;
  int pa = abs(p - a), pb = abs(p - b), pc = abs(p - c);
  if (pa <= pb && pa <= pc) return pa;
  if (pb <= pc) return pc;
  return (x + pc) % 256;
}
int (*GetFilter(int x))(int, int, int, int) {
  switch (x) {
  case 1: return Sub;
  case 2: return Up;
  case 3: return Average;
  case 4: return Paeth;
  default: return None;
  }
}
int GetAbove(int width, int current, char* data) {
  if (current < width) return 0;
  return data[current - width];
}
int GetLeft(int width, int current, char* data) {
  if ((current - 1) % width == 0) return 0;
  return data[current - 1];
}
int GetDiagnal(int width, int current, char* data) {
  if ((current - 1) % width == 0 || current < width) return 0;
  return data[current - 1 - width];
}
Data* Defilter(Data* x, FILE* t) {
  int w = GetWidth(t), h = GetHeight(t), ct = GetColorType(t);
  int rl = w * GetColorByte(ct) + 1;
  Data* res = (Data*)malloc(sizeof(Data));
  res->len = x->len;
  do { res->data = (char*)malloc(x->len + 1); } while (!res->len);

  int (*f)(int, int, int, int);
  for (int i = 0; i < res->len;i++) {
    if (i % rl == 0) {
      f = GetFilter(x->data[i]);
      res->data[i] = x->data[i];
      continue;
    }
    int c = GetDiagnal(rl, i, res->data), b = GetAbove(rl, i, res->data), a = GetLeft(rl, i, res->data);
    res->data[i] = f(a, b, c, x->data[i]);
  }
  return res;
}
void freeData(Data* x) {
  free(x->data);free(x);
}
void PrintPng(FILE* t) {
  int w = GetWidth(t), h = GetHeight(t), ct = GetColorType(t);
  int bd = GetColorByte(ct);
  int rl = w * bd + 1;
  Data* uncom = GetData(t);
  Data* com = Decompress(uncom, t);
  Data* unfil = Defilter(com, t);
  for (int i = 0; i < h;i++) {
    for (int j = 0; j < rl;j++) printf("%5d ", GetReal(unfil->data[i * rl + j], 8));
    printf("\n");
  }
  freeData(uncom);
  freeData(com);
  freeData(unfil);
}
int main() {
  FILE* t = fopen("dd.png", "rb");
  PrintPng(t);
  fclose(t);
}