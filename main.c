#include <stdio.h>
#include <string.h>
#include <math.h>
#include "stdlib.h"

int GetWidth(FILE* t) {
  fseek(t, 8 + 8 + 4, SEEK_SET);
  char w[4]; fread(&w, 1, 4, t);
  for (int i = 0; i < 4;i++)printf("%d ", w[i]);
  return w;
}
int GetHeight() {}
int GetColorType() {}
int GetFilterMethod() {}
int GetBitDepth() {}
int GetInterlac() {

}
char* GetData() {}
char* Decompress() {}

int main() {
  FILE* f = fopen("a.png", "rb");
  printf("%d", GetWidth(f));
  fclose(f);
}