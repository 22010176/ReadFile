#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "../../utils/Number.h"

int main() {
  FILE* t = fopen("a.b", "wb");
  int size = 13;

  for (int i = -128;i < 0;i++) {
    int x = GetReal(i, 8);
    int z = GetBack(x, 8);
    printf("%d %d %d\n", i, x, z);
  }
  // printf("%d %d %d", x, y, z);

}