#include <stdlib.h>

#include "convol.h"

int convol(COMPLEX *data,COMPLEX *kern,int n)
{
   int i,j;

   loop1: for(j = 0; j < n; ++j) {
      loop2: for(i = 0; i < n; ++i) {
         COMPLEX *a = data + j * n + i;
         COMPLEX *b = kern + j * n + i;

         COMPLEX res;
         res.real = a->real * b->real - a->imag * b->imag;
         res.imag = a->real * b->imag + a->imag * b->real;
         res.real /= n;
         res.imag /= n;
         *a = res;
      }
   }

   return(TRUE);
}

int main()
{
   const int ORDER = 4;
   const size_t SIZE = 1 << ORDER;

   COMPLEX *img  = (COMPLEX*)malloc(SIZE * SIZE * sizeof(COMPLEX));
   COMPLEX *kern = (COMPLEX*)malloc(SIZE * SIZE * sizeof(COMPLEX));
   for(size_t y = 0; y < SIZE; ++y) {
      for(size_t x = 0; x < SIZE; ++x) {
         img[y * SIZE + x].real = x;
         img[y * SIZE + x].imag = y;

         kern[y * SIZE + x].real = SIZE - x;
         kern[y * SIZE + x].imag = SIZE - y;
      }
   }

   convol(img,kern,SIZE);

   free(kern);
   free(img);
   return 0;
}
