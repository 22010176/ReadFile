#include <stdio.h>
#include <math.h>
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "utils/utils.h"
#include "zlib/zlib.h"

typedef struct _Scanline Scanline;
struct _Scanline { unsigned int filterType; unsigned char* data; };

typedef struct _Chunk Chunk;
struct _Chunk { unsigned int size; unsigned char* crc, * data, name[5]; };

RGBA* CrRGBA(char r, char g, char b, char a) { return memcpy(_m(sizeof(RGBA)), &(RGBA) { r, g, b, a }, sizeof(RGBA)); }
Data* CrData(size_t len, char* data) { return memcpy(_m(sizeof(Data)), &(Data) { len, data }, sizeof(Data)); }

void FreeChunk(Chunk* x) { free(x->crc); free(x->data); free(x); }
void FreeData(Data* dat) { free(dat->data); free(dat); }

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
  case 0:               // Greyscale
  case 3: return 1;     // Indexed-colour
  case 2: return 3;     // TrueColor
  case 4: return 2;     // Greyscale with alpha
  case 6: return 4;     // Truecolour with alpha
  default: return 0;    // invalid
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

Data* PackedIDAT(FILE* t) {
  size_t cur = 8, i = 0;
  fseek(t, cur, SEEK_SET);
  char* _data = _m(GetFileLen(t) + 1);
  Chunk* data;
  do {
    data = GetChunk(t, cur);
    cur += data->size + 12;
    if (strcmp(data->name, "IDAT")) {
      FreeChunk(data);
      continue;
    }
    memcpy(_data + i, data->data, data->size);
    i += data->size; FreeChunk(data);
  } while (getc(t) != EOF);
  _data[i++] = '\0';
  return CrData(i, Resize(_data, i));
}

int CalcScanline(FILE* t) { return GetWidth(t) * ColorType(GetColorType(t)) + 1; }
int CalcDataLen(FILE* t) { return GetHeight(t) * CalcScanline(t); }
char* Inflate(char* x, size_t len1, size_t len2) {
  char* data = _m(len2);

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

Data* Decompress(FILE* t) {
  Data* idat = PackedIDAT(t); size_t len = CalcDataLen(t) * 2 + 1;
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
char* GetNoInterlackedPNGData(Data* x, int w, int h, int sl) {
  char* _re = x->data, * dat = _m(x->len);
  for (int i = 0, (*f)(int, int, int, int), index; i < h;i++) {
    f = GetFilter(dat[i * sl] = _re[i * sl]);
    for (int j = 1; j < sl;j++) {
      index = i * sl + j;
      dat[index] = f(_re[index], GetLeft(dat, index, sl), GetAbove(dat, index, sl), GetDiagnal(dat, index, sl)) % 256;
    }
  }
  FreeData(x);
  return dat;
}
char* GetInterlackedPNGData(Data* x, int w, int h, int sl) {

} // In progress

char* GetPNGData(FILE* t) {
  Data* x = Decompress(t);
  int h = GetHeight(t), w = GetWidth(t), sl = CalcScanline(t), (*f)(int, int, int, int);
  return (GetInterlanceMethod(t) ? GetInterlackedPNGData : GetNoInterlackedPNGData)(x, w, h, sl);
}
RGBA** GetPLTEPNGData() {} // In progress
RGBA** PackedData(FILE* f) {
  char* data = GetPNGData(f);
  int w = GetWidth(f), h = GetHeight(f), ct = ColorType(GetColorType(f)), sl = CalcScanline(f);
  RGBA** _data = _m(w * h * sizeof(RGBA*));
  for (int i = 0, c = 0; i < w * h;i++) {
    if (i % w == 0) c++;
    _data[i] = memcpy(_m(sizeof(RGBA)), data + i * ct + c, sizeof(RGBA));
  }
  free(data);
  return _data;
}
void PrintPNG(FILE* f) {
  int sl = CalcScanline(f);
  char* data = GetPNGData(f);
  for (int i = 0; i < GetHeight(f);i++) {
    for (int j = 1; j < sl;j++) printf("%5d ", GetReal(data[i * sl + j]));
    printf("\n");
  }
  printf("\n");
}
void WriteFilePNG(FILE* _src, char _dst[]) {
  FILE* f = fopen(_dst, "w+");
  fseek(f, 0, SEEK_SET);

  RGBA** data = PackedData(_src);
  int w = GetWidth(_src), h = GetHeight(_src);
  fwrite((int[]) { w, h }, sizeof(int), 2, f);
  for (int i = 0; i < w * h;i++) {
    fwrite(data[i], sizeof(RGBA), 1, f);
    free(data[i]);
  }
  free(data);
  fclose(f);
}
void PrRGBA(RGBA* a) { printf("%5d %5d %5d %5d\n", a->r, a->g, a->b, a->a); }
IMG* ReadPNGData(char path[]) {
  FILE* f = fopen(path, "rb"); fseek(f, 0, SEEK_SET);

  int s[2]; fread(s, sizeof(int), 2, f);
  int size = s[0] * s[1];

  RGBA** data = _m(size * (sizeof(RGBA*)));
  for (int i = 0; i < size;i++) {
    data[i] = _m(sizeof(RGBA));
    fread(data[i], sizeof(RGBA), 1, f);
  };

  return memcpy(_m(sizeof(IMG)), &(IMG) { s[0], s[1], data }, sizeof(IMG));
}
#if __INCLUDE_LEVEL__ == 0
int main() {
  // printf("%d", GetInterlanceMethod(fopen("imgs/png/test.png", "rb")));
  printf("%d", strlen("1153249531234567890"));
}
#else
#pragma once
#endif