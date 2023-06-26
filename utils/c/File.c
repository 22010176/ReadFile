#include "stdio.h"
#include "string.h"
#include "ctype.h"
#include "math.h"
#include "stdlib.h"
#include "../Number.h"
#include "../header.h"

int GetLen(void** arr) {
  for (int i = 0; 1;i++) if (arr[i] == (void*)-1) return i;
}

int GetSize(FILE* f) {
  size_t cur = ftell(f);
  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  fseek(f, cur, SEEK_SET);
  return size;
}
char* ReadData(FILE* f, int pos, int len) {
  char temp[len];
  fseek(f, pos, SEEK_SET);
  fread(temp, 1, len, f);
  return strcpy((char*)malloc(len), temp);
}
char a(int i, int n) {
  if (i % n) return ' ';
  return '\n';
}
char* GetAllData(FILE* _src) {
  size_t size = GetSize(_src);
  char* a = (char*)malloc(size);
  fseek(_src, 0, SEEK_SET);
  fread(a, 1, size, _src);
  return a;
}
void WriteFile(FILE* _src, FILE* _dest, int indent, int n) {
  int size = GetSize(_src);
  char* data;
  do { data = (char*)malloc(size * 2 + 1); } while (!data);
  fread(data, 1, size, _src);
  for (int i = 0; i < size;i++) fprintf(_dest, "%*x%c", indent, GetReal(data[i], 8), a(i + 1, n));
}

#if __INCLUDE_LEVEL__ == 0
int main() {
  FILE* t, * o;
  char _str[100];

  do { t = fopen("C:/Users/ducmi/Downloads/New folder/data/png/sample2.png", "rb"); } while (!t);
  do { o = fopen("test2.txt", "w+"); } while (!o);
  WriteFile(t, o, 3, 16);
  // for (int i = 0; i < size;i++)printf("%-2x%c", GetReal(x[i], 8), a(i + 1, 8));
}
#else
#pragma once
#endif