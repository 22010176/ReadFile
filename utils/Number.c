#include "math.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"


int GetReal(int x, int p) {
  if (x >= 0) return x;
  return (int)pow(2, p) + x;
}
#if __INCLUDE_LEVEL__ == 0
int main() {

}
#else
#pragma once
#endif