#include <stdlib.h>

#include "fft2d.h"

static float fsqrt(const float x)
{
   union
   {
      int i;
      float x;
   } u;
   u.x = x;
   u.i = (1<<29) + (u.i >> 1) - (1<<22);

   // Two Babylonian Steps (simplified from:)
   // u.x = 0.5f * (u.x + x/u.x);
   // u.x = 0.5f * (u.x + x/u.x);
   u.x =       u.x + x/u.x;
   u.x = 0.25f*u.x + x/u.x;

   return u.x;
}

// source: http://paulbourke.net/miscellaneous/dft/

/*-------------------------------------------------------------------------
   This computes an in-place complex-to-complex FFT
   x and y are the real and imaginary arrays of 2^m points.
   dir =  1 gives forward transform
   dir = -1 gives reverse transform

     Formula: forward
                  N-1
                  ---
              1   \          - j k 2 pi n / N
      X(n) = ---   >   x(k) e                    = forward transform
              N   /                                n=0..N-1
                  ---
                  k=0

      Formula: reverse
                  N-1
                  ---
                  \          j k 2 pi n / N
      X(n) =       >   x(k) e                    = forward transform
                  /                                n=0..N-1
                  ---
                  k=0
*/
static int FFT(int dir,int m,float *rbuf,float *ibuf)
{
   long nn,i,i1,j,k,i2,l,l1,l2;
   float c1,c2,tx,ty,t1,t2,u1,u2,z;

   /* Calculate the number of points */
   nn = 1;
   loop1: for (i=0;i<m;i++)
      nn *= 2;

   /* Do the bit reversal */
   i2 = nn >> 1;
   j = 0;
   loop2: for (i=0;i<nn-1;i++) {
      if (i < j) {
         tx = rbuf[i];
         ty = ibuf[i];
         rbuf[i] = rbuf[j];
         ibuf[i] = ibuf[j];
         rbuf[j] = tx;
         ibuf[j] = ty;
      }
      k = i2;
      loop3: while (k <= j) {
         j -= k;
         k >>= 1;
      }
      j += k;
   }

   /* Compute the FFT */
   c1 = -1.0f;
   c2 = 0.0f;
   l2 = 1;
   loop4: for (l=0;l<m;l++) {
      l1 = l2;
      l2 <<= 1;
      u1 = 1.0f;
      u2 = 0.0f;
      loop5: for (j=0;j<l1;j++) {
         loop6: for (i=j;i<nn;i+=l2) {
            i1 = i + l1;
            t1 = u1 * rbuf[i1] - u2 * ibuf[i1];
            t2 = u1 * ibuf[i1] + u2 * rbuf[i1];
            rbuf[i1] = rbuf[i] - t1;
            ibuf[i1] = ibuf[i] - t2;
            rbuf[i] += t1;
            ibuf[i] += t2;
         }
         z =  u1 * c1 - u2 * c2;
         u2 = u1 * c2 + u2 * c1;
         u1 = z;
      }
      c2 = fsqrt((1.0f - c1) / 2.0f);
      if (dir == 1)
         c2 = -c2;
      c1 = fsqrt((1.0f + c1) / 2.0f);
   }

   /* Scaling for forward transform */
   if (dir == 1) {
      loop7: for (i=0;i<nn;i++) {
         rbuf[i] /= (float)nn;
         ibuf[i] /= (float)nn;
      }
   }

   return(TRUE);
}

/*-------------------------------------------------------------------------
   Perform a 2D FFT inplace given a complex 2D array
   The direction dir, 1 for forward, -1 for reverse
   The size of the array (n,n)
   ibuf/rbuf = buffer of size n * sizeof(float)
   nx = 2^m
   ny = 2^m
*/
int FFT2D(COMPLEX *c,float *rbuf,float *ibuf,int n,int m,int dir)
{
   int i,j;

   /* Transform the rows */
   loop1: for (j=0;j<n;j++) {
      loop2: for (i=0;i<n;i++) {
         rbuf[i] = c[j * n + i].real;
         ibuf[i] = c[j * n + i].imag;
      }
      FFT(dir,m,rbuf,ibuf);
      loop3: for (i=0;i<n;i++) {
         c[j * n + i].real = rbuf[i];
         c[j * n + i].imag = ibuf[i];
      }
   }

   /* Transform the columns */
   loop4: for (i=0;i<n;i++) {
      loop5: for (j=0;j<n;j++) {
         rbuf[j] = c[j * n + i].real;
         ibuf[j] = c[j * n + i].imag;
      }
      FFT(dir,m,rbuf,ibuf);
      loop6: for (j=0;j<n;j++) {
         c[j * n + i].real = rbuf[j];
         c[j * n + i].imag = ibuf[j];
      }
   }

   return(TRUE);
}

int main()
{
   const int ORDER = 4;
   const size_t SIZE = 1 << ORDER;

   COMPLEX *img = (COMPLEX*)malloc(SIZE * SIZE * sizeof(COMPLEX));
   for(size_t y = 0; y < SIZE; ++y) {
      for(size_t x = 0; x < SIZE; ++x) {
         img[y * SIZE + x].real = x;
         img[y * SIZE + x].imag = y;
      }
   }

   float *real = (float *)malloc(SIZE * sizeof(float));
   float *imag = (float *)malloc(SIZE * sizeof(float));

   FFT2D(img,real,imag,SIZE,ORDER,1);

   free(imag);
   free(real);
   free(img);
   return 0;
}
