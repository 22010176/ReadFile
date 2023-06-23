#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>
#include <math.h>
int GetReal(int x) {
  if (x > 0) return x;
  return (int)pow(2, 8) + x;
}
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
void f(FILE* f) {
  char* head = ReadHeader(f, 0, 8);
  for (int i = 0; i < 8;i++) printf("%u ", GetReal(head[i]));
}
char** ReadFile(char* _src) {
  // char data[][100];

  FILE* f = fopen(_src, "rb");
  fseek(f, 0, SEEK_END);
  printf("%d", ftell(f));
}
int main() {
  int num;
  char path[] = "sample.png";
  FILE* fptr = fopen(path, "rb");
  printf("%d", GetSize(fptr));
  if (!fptr) exit(1);

  fclose(fptr);
}