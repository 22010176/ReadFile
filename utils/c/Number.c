#include "math.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
// (int*)memcpy(malloc(len * sizeof(int)), data, len);
int GetReal(int x, int p) {
  if (x >= 0) return x;
  return (int)pow(2, p) + x;
}
int GetBack(int x, int p) {
  if (x < (int)pow(2, p - 1)) return x;
  return x - (int)pow(2, p);
}
int GetIntData(char* x, int l) {
  int _x = 0;
  for (int i = 0; i < l;i++) _x += (int)pow(16, (l - i - 1) * 2) * GetReal(x[i], 8);

  return _x;
}
char* GetStrData(char* x, int l) {
  char a[l + 1];
  for (int i = 0; i <= l;i++)a[i] = i < l ? x[i] : '\0';
  return strcpy((char*)malloc(l + 1), a);
}

#if __INCLUDE_LEVEL__ == 0
int main() {
  // char hex[] = "0123456789ABCDEF";
  // printf("%d", strchr(hex, 'A') - hex);
  GetBack(0, 0);
}
#else
#pragma once
#endif