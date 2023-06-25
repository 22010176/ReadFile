#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "math.h"
#include "stdlib.h"
#include "Number.c"

#include "header.h"

int GetSize(FILE* f) {
  size_t cur = ftell(f);
  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  fseek(f, cur, SEEK_SET);
  return size;
}

char* ReadData(FILE* f, int pos, int len) {
  char* i = malloc(len);
  fseek(f, pos, SEEK_SET);
  fread(i, 1, len, f);
  return i;
}
char* a(int i, int n) {
  if (i % n) return " ";
  return "\n";
}
void WriteFile(FILE* _src, FILE* _dest, int indent, int n) {
  int size = GetSize(_src);
  char* data;
  do { data = malloc(size * 2 + 1); } while (!data);
  fread(data, 1, size, _src);
  for (int i = 0; i < size;i++) fprintf(_dest, "%*x%s", indent, GetReal(data[i], 8), a(i + 1, n));
}
#if __INCLUDE_LEVEL__ == 0
int main() {
  FILE* t, * o;
  char _str[100];

  do { t = fopen("./../data/png/sample2.png", "rb"); } while (!t);
  do { o = fopen("test.txt", "w+"); } while (!o);

  WriteFile(t, o, 2, 16);
}
#else
#pragma once
#endif