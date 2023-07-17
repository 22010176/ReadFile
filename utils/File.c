#include "stdio.h"
#include "conio.h"
#include "math.h"
#include "string.h"
#include "ctype.h"

// #include "utils.h"
FILE* Fopen(char* s, char* _t) {
    FILE* t; do t = fopen(s, _t); while (!t);
    return t;
}
void WriteFileDatA(char* _s, char* _d) {
    FILE* s = Fopen(_s, "rb"), * d = Fopen(_d, "w+");
    char ch; int i = 0;
    do {
        fread(&ch, 1, 1, s);
        // if (isalnum(ch)) 
        fprintf(d, "%c", ch);
        // else fprintf(d, ".");
        // if (++i % 32 == 0) fprintf(d, "\n");
    } while (!feof(s));
    fclose(s); fclose(d);
}
#if __INCLUDE_LEVEL__ == 0
int main() {
}
#else
#pragma once
#endif