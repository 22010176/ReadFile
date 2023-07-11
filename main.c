#include <stdio.h>
#include <math.h>
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include "zlib/zlib.h"

typedef struct _PNG PNG;
struct _PNG {
  int width, height, bitdepth, colortype, compression, filter, interlance;
  char* data;
};

typedef struct _Chunk Chunk;
struct _Chunk { int size; char* crc, * data, name[5]; };

typedef struct _Data Data;
struct _Data { size_t len; char* data; };

int GetReal(int x) {
  if (x >= 0) return x;
  return (int)pow(2, 8) + x;
}
int GetInt(char* x, int l) {
  int z = 0;
  for (int i = 0; i < l;i++) z += GetReal(x[l - i - 1]) * (int)pow(16, i * 2);
  return z;
}

char* ReadData(FILE* t, size_t pos, size_t len) {
  char* res = malloc(len);
  fseek(t, pos, SEEK_SET);
  fread(res, 1, len, t);
  return res;
}

int GetWidth(FILE* t) {
  char* w = ReadData(t, 16, 4);
  int e = GetInt(w, 4);
  free(w); return e;
}
int GetHeight(FILE* t) {
  char* w = ReadData(t, 20, 4);
  int e = GetInt(w, 4);
  free(w); return e;
}
int GetBitDepth(FILE* t) {
  char* w = ReadData(t, 24, 1);
  int e = GetInt(w, 1);
  free(w); return e;
}
int GetColorType(FILE* t) {
  char* w = ReadData(t, 25, 1);
  int e = GetInt(w, 1);
  free(w); return e;
}
int GetCompressionMethod(FILE* t) {
  char* w = ReadData(t, 26, 1);
  int e = GetInt(w, 1);
  free(w); return e;
}
int GetFilterMethod(FILE* t) {
  char* w = ReadData(t, 27, 1);
  int e = GetInt(w, 1);
  free(w); return e;
}
int GetInterlanceMethod(FILE* t) {
  char* w = ReadData(t, 28, 1);
  int e = GetInt(w, 1);
  free(w); return e;
}

int ColorType(int x) {
  switch (x) {
  case 0:
  case 3: return 1;
  case 2: return 3;
  case 4: return 2;
  case 6: return 4;
  default: return 0;
  }
}

Chunk* GetChunk(FILE* t, size_t pos) {
  Chunk* a = malloc(sizeof(Chunk));

  char* data = ReadData(t, pos, 4);
  a->size = GetInt(data, 4);
  free(data);

  data = ReadData(t, pos + 4, 5);
  data[4] = '\0';
  strcpy(a->name, data);
  free(data);

  a->data = ReadData(t, pos + 8, a->size);
  a->crc = ReadData(t, pos + 8 + a->size, 4);

  return a;
}
void FreeData(Chunk* x) {
  free(x->crc);
  free(x->data);
  free(x);
}
void* Resize(void* mem, size_t new) {
  void* a;
  do a = realloc(mem, new); while (!a);
  return a;
}
Data* GetIDAT(FILE* t) {
  size_t cur = 33, len = 1, i = 0;
  Chunk* data;
  char* x = malloc(len);
  while (1) {
    data = GetChunk(t, cur);
    size_t size = data->size;
    cur += size + 12;
    if (!strcmp(data->name, "IDAT")) {
      while (i + size >= len) x = Resize(x, len *= 2);
      memcpy(x + i, data->data, size);
      i += size;
    }
    FreeData(data);
    if (!strcmp(data->name, "IEND")) break;
  }
  FreeData(data);
  Data* res = malloc(sizeof(Data));
  res->len = i, res->data = Resize(x, i);
  return res;
}
int CalcDataLen(int w, int h, int ct) { return  w * h * ColorType(ct) + h; }
Data* Decompress(FILE* t) {
  Data* idat = GetIDAT(t), * res;
  do res = malloc(sizeof(Data)); while (!res);
  int len = CalcDataLen(GetWidth(t), GetHeight(t), GetColorType(t));
  res->len = len;
  do res->data = malloc(len); while (!res->data);

  z_stream a;
  a.zalloc = Z_NULL;
  a.zfree = Z_NULL;
  a.opaque = Z_NULL;
  a.avail_in = (uInt)(idat->len);
  a.next_in = (Bytef*)idat->data;
  a.avail_out = (uInt)len;
  a.next_out = (Bytef*)res->data;

  inflateInit(&a);
  inflate(&a, Z_NO_FLUSH);
  inflateEnd(&a);

  return res;
}

int None() {}
int Sub() {}
int Up() {}
int Average() {}
int Paeth() {}

int GetAbove() {}
int GetLeft() {}
int GetDiagnal() {}

char* Defilter() {}

void PrintPNG() {}
void WritePNG() {}
void Testing() {}

int main() {
  char path[] = "ae.png";
  FILE* f = fopen(path, "rb");
  int w = GetWidth(f), ct = ColorType(GetColorType(f));
  Data* x = Decompress(f);

  printf("%d", x->len);
}