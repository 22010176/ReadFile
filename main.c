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
  int e = GetINT(w, 4);
  free(w); return e;
}
int GetHeight(FILE* t) {
  char* w = ReadData(t, 20, 4);
  int e = GetINT(w, 4);
  free(w); return e;
}
int GetBitDepth(FILE* t) {
  char* w = ReadData(t, 24, 4);
  int e = GetINT(w, 4);
  free(w); return e;
}
int GetColorType(FILE* t) {
  char* w = ReadData(t, 25, 4);
  int e = GetINT(w, 4);
  free(w); return e;
}
int GetCompressionMethod(FILE* t) {
  char* w = ReadData(t, 26, 4);
  int e = GetINT(w, 4);
  free(w); return e;
}
int GetFilterMethod(FILE* t) {
  char* w = ReadData(t, 27, 4);
  int e = GetINT(w, 4);
  free(w); return e;
}
int GetInterlanceMethod(FILE* t) {
  char* w = ReadData(t, 28, 4);
  int e = GetINT(w, 4);
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
char* GetIDAT(FILE* t) {

}
char* Decompress() {}


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

}