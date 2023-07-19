#include "stdlib.h"
#include "stdio.h"
#include "math.h"

typedef struct _Data Data;
struct _Data { size_t len; unsigned char* data; };

typedef struct _RGBA RGBA;
struct _RGBA { unsigned char r, g, b, a; }; // usigned char [4]

typedef struct _IMG IMG;
struct _IMG { unsigned int width, height; RGBA** data; }; // Do not change


void* _m(size_t _size) {
  void* mem; do mem = malloc(_size); while (!mem);
  return mem;
}

char* ReadData(FILE* t, size_t pos, size_t len) {
  char* res = _m(len);
  fseek(t, pos, SEEK_SET);
  fread(res, 1, len, t);
  return res;
}

void* Resize(void* mem, size_t n) {
  void* a; do a = realloc(mem, n); while (!a);
  return a;
}

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
