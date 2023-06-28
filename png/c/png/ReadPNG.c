#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

#include "../../zlib-master/zlib.h"
#include "../../Chunk.h"
#include "../../../utils/File.h"
#include "../../../utils/Number.h"

typedef struct color {
  int red, green, blue, alpha;
} Color;
typedef struct image IMG;
struct image {
  int width, height;
  Color** data;
};

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
Chunk** GetAllChunk(FILE* t, int n) {
  int clen = 4;
  int fsize = GetSize(t), i = 0;
  Chunk** x;
  do { x = (Chunk**)malloc(clen * sizeof(Chunk*)); } while (!x);
  x[i++] = _Chunk(8, 0, (char*)"header");
  x[0]->SetData(x[0], ReadData(t, 0, 8), x[0]->size);
  x[0]->SetCrf(x[0], (char*)"0000");
  for (int j = ftell(t);getc(t) != EOF && i < n;j = ftell(t)) {
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
  int output_size = Psize[0] * Psize[1] * 20;
  do { output = (char*)malloc(output_size); } while (!output);
  z_stream  out;
  out.zalloc = Z_NULL;
  out.zfree = Z_NULL;
  out.opaque = Z_NULL;

  out.avail_in = (uInt)(len);
  out.next_in = (Bytef*)data;
  out.avail_out = (uInt)(output_size);
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
  _data[len] = '\0';
  return _data;
}
int* GetPNGScale(Chunk* IHDR) {
  int size[2] = { GetIntData(IHDR->data, 4),GetIntData(IHDR->data + 4, 4) };
  return (int*)memcpy(malloc(2 * sizeof(int)), size, 2 * sizeof(int));
}
char* Decompress(Chunk** data) {
  Chunk** idat = GetIdat(data);
  int* size = GetPNGScale(data[1]), _d = GetIdatSize(data), _l = GetLen((void**)idat);

  char* _data = PackedData(idat, _l);
  char* output = InflateData(_data, _d, size);

  free(idat);
  free(size);
  free(_data);

  return output;
}
void WritePNGData(FILE* _dest, Chunk** data) {
  int len = GetLen((void**)data);
  for (int i = 0; i < len;i++) data[i]->FPrint(data[i], _dest, 1);
}


#if __INCLUDE_LEVEL__ == 0
int main() {
  char a[] = "C:/Users/ducmi/Downloads/New folder/data/png/sample.png";
  char b[] = "Ext.txt";
  FILE* t = fopen(a, "rb");
  FILE* o = fopen(b, "wb");
  Chunk** _a = GetAllChunk(t, 2000000000);
  WritePNGData(o, _a);
  // _a[1]->Print(_a[1], 1);
  for (int i = 0; _a[i] != (Chunk*)-1;i++) _a[i]->Free(_a[i]);
  free(_a);
  char* e = Decompress(_a);
  for (int i = 0; i < 1000;i++) printf("%3d ", e[i]);
  // free(size);
  free(e);
}
#else
#pragma once
#endif