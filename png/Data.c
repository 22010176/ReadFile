#include "stdio.h"
#include "stdlib.h"
#include "conio.h"
#include "math.h"
#include "string.h"

typedef struct _da {
  char* data, name[101];
  int len, pos, (*GetData)();
} Data;
static Data* _Create() {

}
#if __INCLUDE_LEVEL__ == 0
int main() {

}
#else
#pragma once
#endif