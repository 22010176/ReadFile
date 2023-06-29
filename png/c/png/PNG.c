#include "stdio.h"
#include "conio.h"
#include "string.h"
#include "ctype.h"
#include "stdlib.h"

#include "../../ReadPNG.h"
#include "../../Chunk.h"

typedef struct _png PNG;
struct _png {
  int width, height, bit_depth, colour_type, compression_method, filter_method, interlance_method;
  Chunk** critical, ancillary;

};


#if __INCLUDE_LEVEL == 0
int main() {

}
#else
#pragma once
#endif