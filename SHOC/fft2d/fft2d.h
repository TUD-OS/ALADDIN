typedef struct {
   float real;
   float imag;
} COMPLEX;

#define TRUE 1
#define FALSE 0

int FFT2D(COMPLEX *c,float *rbuf,float *ibuf,int n,int m,int dir);
