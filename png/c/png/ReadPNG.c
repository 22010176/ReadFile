#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

#include "../../zlib-master/zlib.h"
#include "../../Chunk.h"
#include "../../../utils/File.h"
#include "../../../utils/Number.h"


Chunk* ReadChunk(FILE* t, int pos) {
  char size[4], name[5], crf[5];
  fseek(t, pos, SEEK_SET);
  fread(size, 1, 4, t), fread(name, 1, 4, t), name[4] = '\0';
  int _s = GetIntData(size, 4);
  char data[_s + 1];
  fread(data, 1, _s, t), fread(crf, 1, 4, t);
  Chunk* x = _Chunk(_s, pos, name);
  x->SetData(x, data, _s), x->SetCrf(x, crf);

  return x;
}
Chunk** GetAllChunk(FILE* t) {
  int clen = 4;
  int fsize = GetSize(t), i = 0;
  Chunk** x;
  do { x = (Chunk**)malloc(clen * sizeof(Chunk*)); } while (!x);
  x[i++] = _Chunk(8, 0, (char*)"header");
  x[0]->SetData(x[0], ReadData(t, 0, 8), x[0]->size);
  x[0]->SetCrf(x[0], (char*)"0000");
  for (int j = ftell(t);getc(t) != EOF;j = ftell(t)) {
    x[i++] = ReadChunk(t, j);
    if (i < clen) continue;
    clen *= 2;
    do { x = (Chunk**)realloc(x, (clen + 1) * sizeof(Chunk*)); } while (!x);
  }
  x[i++] = (Chunk*)-1;
  do { x = (Chunk**)realloc(x, i * sizeof(Chunk*)); } while (!x);
  return x;
}

Chunk** GetIdat(Chunk** d) {
  int len = GetLen((void**)d);
  Chunk** res;
  do { res = (Chunk**)malloc(len * sizeof(Chunk*)); } while (!res);
  int i, j; for (i = j = 0; i < len;i++) {
    if (strcmp(d[i]->name, "IDAT")) continue;
    res[j++] = d[i];
  }
  res[j++] = (Chunk*)-1;
  return (Chunk**)realloc((void*)res, j * sizeof(Chunk*));
}
int  GetIdatSize(Chunk** d) {
  int x = 0, l = GetLen((void**)d);
  for (int i = 0; i < l;i++) {
    if (strcmp(d[i]->name, "IDAT")) continue;
    x += d[i]->size;
  }
  return x;
}
char* InflateData(char* data, int len, int* Psize) {
  char* output;
  int output_size = Psize[0] * Psize[1] * 4 + Psize[1];
  do { output = (char*)malloc(output_size); } while (!output);

  z_stream  out;
  out.zalloc = Z_NULL;
  out.zfree = Z_NULL;
  out.opaque = Z_NULL;

  out.avail_in = (uInt)(len + 1);
  out.next_in = (Bytef*)data;
  out.avail_out = (uInt)(output_size + 1);
  out.next_out = (Bytef*)output;

  inflateInit(&out);
  inflate(&out, Z_NO_FLUSH);
  inflateEnd(&out);

  return output;
}
char* PackedData(Chunk** data, int len) {
  char* _data;
  int j = 0, size = GetIdatSize(data);
  do { _data = (char*)malloc(size + 1); } while (!_data);

  for (int i = 0;i < len;i++) {
    if (strcmp(data[i]->name, "IDAT")) continue;
    Chunk* x = data[i];
    memcpy(_data + j, x->data, x->size);
    j += x->size;
  }
  _data[size] = '\0';
  return _data;
}
int calcDataLen(int* size) { return size[1] * size[0] * 4 + size[1]; }
int* GetPNGScale(Chunk* IHDR) {
  int size[2] = { GetIntData(IHDR->data, 4),GetIntData(IHDR->data + 4, 4) };
  return (int*)memcpy(malloc(2 * sizeof(int)), size, 2 * sizeof(int));
}
char* Decompress(Chunk** data) {
  Chunk** idat = GetIdat(data);
  int* size = GetPNGScale(data[1]);

  char* _data = PackedData(idat, GetLen((void**)idat));
  char* output = InflateData(_data, GetIdatSize(data), size);

  free(idat);
  free(size);
  free(_data);

  return output;
}
void WritePNGData(FILE* _dest, Chunk** data) {
  int len = GetLen((void**)data);
  for (int i = 0; i < len;i++) {
    data[i]->FPrint(data[i], _dest, 1);
  }
  fflush(_dest);
}

void FPrintPNGData(FILE* _dest, char* output, int* size) {
  int indent = size[0] * 4 + 1, len = calcDataLen(size);
  for (int i = 0; i < len;i++) fprintf(_dest, "%2x%c", GetReal(output[i], 8), AddIndent(i + 1, indent));
  fflush(_dest);
}
char** Create2dArr(int x, int y) {
  char** res;
  do { res = (char**)malloc(y); } while (!res);
  for (int i = 0; i < y;i++) {
    do {
      res[i] = (char*)malloc(x + 1);
      printf("%d %d %d\n", i, y, !res[i]);
    } while (!res[i]);
  }
  return res;
}
char** _1Dto2D(char* arr, int l, int* size) {
  int _row = size[0] * 4 + 1;
  char** res = Create2dArr(_row, size[1]);
  for (int i = 0; i < size[1];i++) for (int j = 0; j < _row;j++) res[i][j] = arr[_row * i + j];
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
int(*PickFilter(int t))(int, int, int, int) {
  switch (t) {
  case 0: return None;
  case 1: return Sub;
  case 2: return Up;
  case 3: return Average;
  case 4: return Paeth;
  default: (int(*)(int, int, int, int)) - 1;
  }
}

#if __INCLUDE_LEVEL__ == 0
int main() {
  char a[] = "C:/Users/ducmi/Downloads/New folder/data/png/sample.png";

  FILE* t = fopen(a, "rb");
  FILE* z = fopen("aeee.txt", "w");
  Chunk** _a = GetAllChunk(t);

  WritePNGData(z, _a);
  char* e = Decompress(_a);
  int* size = GetPNGScale(_a[1]);

  for (int i = 0; _a[i] != (Chunk*)-1;i++) _a[i]->Free(_a[i]);
  free(_a);

  char** d = _1Dto2D(e, calcDataLen(size), size);
  printf("HE");
  // char** d2 = Create2dArr(size[0] * 4 + 1, size[1]);

  // for (int i = 0; i < size[1];i++) {
  //   int (*f)(int, int, int, int) = PickFilter(d[i][0]);
  //   for (int j = 1; j < size[0] * 4 + 1;j++) {

  //   }
  // }


  for (int i = 0; i < size[1];i++) {
    for (int j = 0; j < 4;j++) printf("%4d", GetReal(e[i * (size[0] * 4 + 1) + j], 8));
    printf("\n");
    // for (int j = 1; j < size[0];j++) {
    //   for (int k = 0; k < 4;k++)printf("%x", GetReal(d[i][j + k], 8));
    // }
    // printf("\n");
  }
  // for (int i = 0;i < size[1];i++) {
  //   free(d[i]);
  //   free(d2[i]);
  // }
  fclose(t);
  fclose(z);
  // free(d);
  // free(d2);

  free(size);
  free(e);
}
#else
#pragma once
#endif