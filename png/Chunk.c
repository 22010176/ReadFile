#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "stdlib.h"
#include "Data.h"
#include "../utils/File.h"

typedef struct _ch {
  Data** data;
  int len, pos, crf[4];
  char name[5];
  void (*_f)(void**, int);
} Chunk;

Chunk* CrCh(int pos) {
  int size[5];
  char name[5];
}
#if __INCLUDE_LEVEL__ == 0
int main() {
  Chunk a;
}
#else
#pragma once
#endif