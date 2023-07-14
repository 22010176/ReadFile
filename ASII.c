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

typedef struct _pixel APixel;
struct _pixel { int intent; }; // 0 -> 255

typedef struct _screen AScreen;
struct _screen {
  int width, height, wr, hr;
  APixel** screen;
  char* path;
};
float rgb2gc(RGBA* color) { return (int)(0.299 * color->r + 0.587 * color->g + 0.114 * color->b); }

APixel* CrPixel(int intent) { return memcpy(_m(sizeof(APixel)), &(APixel) { intent }, sizeof(APixel)); }
AScreen* CrScreen(int width, int height, int wr, int hr) { return memcpy(_m(sizeof(AScreen)), &(AScreen) { width, height, wr, hr, NULL, NULL }, sizeof(AScreen)); }

APixel** Process(RGBA** data, int len) {
  APixel** res = _m(len * sizeof(APixel*));
  for (int i = 0; i < len;i++) {
    RGBA* color = _rgba2rgb(data[i], &(RGBA){255, 255, 255, 255});
    res[i] = CrPixel(rgb2gc(color));
    free(color);
  }
  return res;
}
int GetGrey(int intent, char* level) { return level[(int)(intent * (strlen(level) - 1) / 255)]; }
char* Render(AScreen* data, char path[], int cut[]) {
  int w = data->width, h = data->height, wr = data->wr, hr = data->hr;
  FILE* ouput = fopen(path, "w+");
  APixel** sr = data->screen;
  char* res = _m(w * wr * h * hr + h * hr + 1); res[0] = '\0';

  for (int i = cut[0]; i < h - cut[1];i++) {
    char* line = _m(w * wr + 2); line[0] = '\0';
    int k = 0;
    for (int j = cut[2]; j < w - cut[3];j++) {
      char x = GetGrey(sr[i * w + j]->intent, GREYSCALE3);
      for (int l = 0;l < wr;l++) line[k++] = x;
    }
    line[k++] = '\0';
    fprintf(ouput, "%s\n", line);
    free(line);
  }
  fclose(ouput);
  return path;
}
AScreen* RenderASII(char path[], IMG* data, int wr, int hr) {
  AScreen a = { data->width,data->height,wr,hr };
  a.screen = Process(data->data, data->width * data->height);
  a.path = Render(&a, path, (int[]) { 0, 0, 0, 0 });

  return memcpy(_m(sizeof(a)), &a, sizeof(a));
}
int main() {
  WriteFilePNG(fopen("imgs/png/ae.png", "rb"), "b.dat");
  IMG* data = ReadPNGData("b.dat");

  int len = data->height * data->width;

  AScreen* sc = RenderASII("output.txt", data, 2, 1);
  printf("GE");
}