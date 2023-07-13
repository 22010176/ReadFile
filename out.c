#include "stdio.h"
#include "conio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "ctype.h"

#include "PNG.c"
#include "utils/utils.h"

#define GREYSCALE "@MBHENR#KWXDFPQASUZbdehx*8Gm&04LOVYkpq5Tagns69owz$CIu23Jcfry%1v7l+it[] {}?j|()=~!-/<>\"^_';,:`. "

typedef struct _pixel Pixel;
struct _pixel { int intent; };

typedef struct _screen Screen;
struct _screen {
  int width, height, wr, hr;
  Pixel*** screen;
  char* result;
};
float rgb2gc(RGBA* color) { return (0.299 * color->r + 0.587 * color->g + 0.114 * color->b); }

Pixel* CrPixel(int intent) { return memcpy(_m(sizeof(Pixel)), &(Pixel) { intent }, sizeof(Pixel)); }
Screen* CrScreen(int width, int height, int wr, int hr) { return memcpy(_m(sizeof(Screen)), &(Screen) { width, height, wr, hr, NULL, NULL }, sizeof(Screen)); }

Pixel*** Process(RGBA*** data, int h, int w) {
  Pixel*** res = _m(sizeof(h * sizeof(Pixel**)));
  for (int i = 0; i < h;i++) {
    res[i] = _m(w * sizeof(Pixel*));
    for (int j = 0; j < w;j++) res[i][j] = CrPixel(rgb2gc(data[i][j]) / 255 * strlen(GREYSCALE));
  }
  return res;
}
char* Render(Screen* data) {
  char* a = _m((data->width * data->wr + 1) * data->height * data->hr + 1);
  int index = 0;
  for (int i = 0; i < data->height;i++) {
    for (int j = 0; j < data->width;j++) {

    }
    a[index++] = '\n';
  }
  a[index++] = '\0';
  return Resize(a, index);
}
Pixel** FormatPixel(FILE* a) {

}
void*** Create2dArr(int x, int y) {

}
int main() {
  FILE* f = fopen("a.png", "rb");
  FILE* o = fopen("b.bin", "w+");
  WriteFilePNG(f, o);

  int w = GetWidth(f), h = GetHeight(f);
  fseek(o, 0, SEEK_SET);
  RGBA* _data = _m(sizeof(RGBA));
  int i = 0;
  do {
    fread(_data, sizeof(RGBA), 1, o);
    // printf("%5d %5d %5d %5d\n", _data->r, _data->g, _data->b, _data->a);
    printf("%d.\n", ++i);
  } while (!feof(o));

  fclose(f); fclose(o);

  printf("Hello");
}