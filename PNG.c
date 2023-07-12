#include <stdio.h>
#include <math.h>
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include "zlib/zlib.h"

typedef struct _Scanline Scanline;
struct _Scanline { int filterType;  char* data; };

typedef struct _PNG PNG;
struct _PNG { int width, height, bitdepth, colortype, compression, filter, interlance; char* data; };

typedef struct _RGBA RGBA;
struct _RGBA { short r, g, b, a; };

typedef struct _Chunk Chunk;
struct _Chunk { int size; char* crc, * data, name[5]; };

typedef struct _Data Data;
struct _Data { size_t len; char* data; };

size_t GetFileLen(FILE* t) {
  fseek(t, 0, SEEK_END);
  return ftell(t);
}

int GetReal(int x) { return  x >= 0 ? x : (int)pow(2, 8) + x; }
int GetInt(char* x, int l) {
  int z = 0;
  for (int i = 0; i < l;i++) z += GetReal(x[l - i - 1]) * (int)pow(16, i * 2);
  return z;
}

RGBA* CrRGBA(short r, short g, short b, short a) {
  RGBA* color = malloc(sizeof(RGBA));
  color->a = a, color->r = r, color->g = g, color->b = b;
  return color;
}

char* ReadData(FILE* t, size_t pos, size_t len) {
  char* res; do res = malloc(len); while (!res);
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

  data = ReadData(t, pos + 4, 4);
  memcpy(a->name, data, 4);
  a->name[4] = '\0';
  free(data);

  a->data = ReadData(t, pos + 8, a->size);
  a->crc = ReadData(t, pos + 8 + a->size, 4);

  return a;
}
void FreeChunk(Chunk* x) { free(x->crc); free(x->data); free(x); }
void* Resize(void* mem, size_t new) {
  void* a;
  do a = realloc(mem, new); while (!a);
  return a;
}
Data* CrData(size_t len, char* data) {
  Data* _d = malloc(sizeof(Data));
  _d->len = len, _d->data = data;
  return _d;
}
Data* GetIDAT(FILE* t) {
  size_t cur = 8, i = 0;
  fseek(t, cur, SEEK_SET);
  char* _data;
  do _data = malloc(GetFileLen(t) + 1); while (!_data);
  Chunk* data;
  do {
    data = GetChunk(t, cur);
    cur += data->size + 12;
    if (strcmp(data->name, "IDAT")) {
      FreeChunk(data);
      continue;
    }
    memcpy(_data + i, data->data, data->size);
    i += data->size;
    FreeChunk(data);
  } while (getc(t) != EOF);
  _data[i++] = '\0';
  return CrData(i, Resize(_data, i));
}
int CalcScanline(FILE* t) { return GetWidth(t) * ColorType(GetColorType(t)) + 1; }
int CalcDataLen(FILE* t) { return GetHeight(t) * CalcScanline(t); }
char* Inflate(char* x, size_t len1, size_t len2) {
  char* data; do data = malloc(len2); while (!data);

  z_stream a;
  a.zalloc = Z_NULL;
  a.zfree = Z_NULL;
  a.opaque = Z_NULL;

  a.avail_in = (uInt)(len1);
  a.next_in = (Bytef*)x;
  a.avail_out = (uInt)len2;
  a.next_out = (Bytef*)data;

  inflateInit(&a);
  inflate(&a, Z_NO_FLUSH);
  inflateEnd(&a);

  return data;
}
void FreeData(Data* dat) { free(dat->data); free(dat); }
Data* Decompress(FILE* t) {
  Data* idat = GetIDAT(t); size_t len = CalcDataLen(t) * 2 + 1;
  Data* res = CrData(len, Inflate(idat->data, idat->len, len));
  FreeData(idat);
  return res;
}

int None(int index, int left, int up, int diagnal) { return index; }
int Sub(int index, int left, int up, int diagnal) { return index + left; }
int Up(int index, int left, int up, int diagnal) { return index + up; }
int Average(int index, int left, int up, int diagnal) { return index + (int)floor((left + up) / 2); }
int Paeth(int index, int left, int up, int diagnal) {
  int p = left + up - diagnal;
  int pa = abs(p - left), pb = abs(p - up), pc = abs(p - diagnal);
  if (pa <= pb && pa <= pc) return left;
  if (pb <= pc) return up;
  return diagnal;
}

int GetAbove(char* x, int i, int sl) { return i < sl ? 0 : x[i - sl]; }
int GetLeft(char* x, int i, int sl) { return (i - 1) % sl == 0 ? 0 : x[i - 1]; }
int GetDiagnal(char* x, int i, int sl) { return (i < sl || (i - 1) % sl == 0) ? 0 : x[i - sl - 1]; }

int (*GetFilter(int x))(int, int, int, int) {
  switch (x) {
  case 1: return Sub;
  case 2: return Up;
  case 3: return Average;
  case 4: return Paeth;
  default: return None;
  }
}
int _e(float a, int _bg, int _src) { return (int)(_bg * (1 - a) + _src * a); }
RGBA* _rgba2rgb(RGBA* _src, RGBA* _bg) {
  float alpha = (float)_src->a / 255;
  return CrRGBA(_e(alpha, _bg->r, _src->r), _e(alpha, _bg->g, _src->g), _e(alpha, _bg->b, _src->b), 255);
}
int CheckIndex(int x) { return x == -1 ? 0 : x; }
char* Defilter(FILE* t) {
  Data* x = Decompress(t);
  int h = GetHeight(t), w = GetWidth(t), sl = CalcScanline(t), (*f)(int, int, int, int);
  char* _re = x->data, * dat;
  do dat = malloc(x->len); while (!dat);
  for (int i = 0, index; i < h;i++) {
    f = GetFilter(dat[i * sl] = _re[i * sl]);
    for (int j = 1; j < sl;j++) {
      index = i * sl + j;
      dat[index] = f(_re[index], GetLeft(dat, index, sl), GetAbove(dat, index, sl), GetDiagnal(dat, index, sl)) % 256;
    }
  }
  FreeData(x);
  return dat;
}
RGBA*** PackedData(FILE* f) {
  int w = GetWidth(f), h = GetHeight(f), ct = ColorType(GetColorType(f)), sl = CalcScanline(f);
  RGBA*** x = malloc(h * sizeof(RGBA**));
  char* data = Defilter(f);
  for (int i = 0; i < h;i++) {
    x[i] = malloc(w * sizeof(RGBA*));
    for (int j = 0; j < w;j++) {
      x[i][j] = &(RGBA) { 0, 0, 0, 255 };
      for (int k = 0; k < ct;k++) *((short*)x[i][j] + k) = GetReal(data[1 + sl * i + ct * j + k]) % 256;
      x[i][j] = _rgba2rgb(x[i][j], &(RGBA){255, 255, 255, 0});
    }
  }
  free(data);
  return x;
}
void PrintPNG(FILE* f, char* data) {
  int sl = CalcScanline(f);
  for (int i = 0; i < GetHeight(f);i++) {
    for (int j = 0; j < sl;j++) printf("%4d", GetReal(data[i * sl + j]));
    printf("\n");
  }
  printf("\n");
}
void WriteFilePNG(FILE* _src, FILE* _dst) {
  RGBA*** data = PackedData(_src);
  int w = GetWidth(_src), h = GetHeight(_src);
  fflush(_dst);
  short r, g, b, a;
  for (int i = 0; i < h;i++) {
    for (int j = 0; j < w;j++) {
      r = data[i][j]->r, g = data[i][j]->g, b = data[i][j]->b, a = data[i][j]->a;
      printf("%5d %5d %5d %5d", r, g, b, a);
      fwrite(&r, sizeof(short), 1, _dst);
      fwrite(&g, sizeof(short), 1, _dst);
      fwrite(&b, sizeof(short), 1, _dst);
      fwrite(&a, sizeof(short), 1, _dst);
      free(data[i][j]);
    }
    printf("\n");
    free(data[i]);
  }
  free(data);
  printf("\n");
}
#if __INCLUDE_LEVEL__ == 0
int main() {
  FILE* f = fopen("a.png", "rb");
  FILE* o = fopen("b.bin", "w+");
  WriteFilePNG(f, o);
}
#else
#pragma once
#endif