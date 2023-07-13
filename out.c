#include "stdio.h"
#include "conio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "ctype.h"

#include "PNG.c"
#include "utils/utils.h"

#define GREYSCALE "@MBHENR#KWXDFPQASUZbdehx*8Gm&04LOVYkpq5Tagns69owz$CIu23Jcfry%1v7l+it[]{}?j|()=~!-/<>\"^_';,:`. "
#define GREYSCALE2 "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. "
#define GREYSCALE3 "@%#*+=-:. "

typedef struct _pixel Pixel;
struct _pixel { int intent; }; // 0 -> 255

typedef struct _screen Screen;
struct _screen {
  int width, height, wr, hr;
  Pixel** screen;
  char* result;
};
float rgb2gc(RGBA* color) { return (int)(0.299 * color->r + 0.587 * color->g + 0.114 * color->b); }

Pixel* CrPixel(int intent) { return memcpy(_m(sizeof(Pixel)), &(Pixel) { intent }, sizeof(Pixel)); }
Screen* CrScreen(int width, int height, int wr, int hr) { return memcpy(_m(sizeof(Screen)), &(Screen) { width, height, wr, hr, NULL, NULL }, sizeof(Screen)); }

Pixel** Process(RGBA** data, int len) {
  Pixel** res = _m(len * sizeof(Pixel*));
  for (int i = 0; i < len;i++) {
    RGBA* color = _rgba2rgb(data[i], &(RGBA){255, 255, 255, 255});
    res[i] = CrPixel(rgb2gc(color));
    free(color);
  }
  return res;
}
char* Render(Screen* data, char path[], int cut[]) {
  FILE* f = fopen(path, "w+");
  int w = data->width, h = data->height, wr = data->wr, hr = data->hr;

  Pixel** sr = data->screen;
  for (int i = cut[0], k; i < h - cut[1];i++, k = 0) {
    char* line = _m(w * wr * 2 + 1);
    for (int j = cut[2]; j < w - cut[3];j++) {
      char x = GREYSCALE3[(int)(sr[i * w + j]->intent * strlen(GREYSCALE3) / 255)];
      for (int l = 0;l < wr;l++) line[k++] = x;
    }
    line[k++] = '\0';
    for (int l = 0; l < hr;l++) fprintf(f, "%s\n", line);
    free(line);
  }
}
int main() {
  WriteFilePNG(fopen("Untitled.png", "rb"), "b.dat");
  IMG* data = ReadPNGData("b.dat");
  int len = data->height * data->width;

  Screen* sc = CrScreen(data->width, data->height, 2, 1);
  sc->screen = Process(data->data, len);
  Render(sc, "output.txt", (int[]) { 0, 0, 50, 50 });
  printf("Finish");
}