#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "fft.h"

int main(int argc, char **argv)
{
  struct bench_args_t data;
  int i, fd;
  struct prng_rand_t state;

  // Fill data structure
  prng_srand(1,&state);
  for(i=0; i<DATA_LEN; i++){
    data.in_x[i] = ((TYPE)prng_rand(&state))/((TYPE)PRNG_RAND_MAX);
    data.in_y[i] = ((TYPE)prng_rand(&state))/((TYPE)PRNG_RAND_MAX);
    data.out_x[i] = 0;
    data.out_y[i] = 0;
  }

  // Open and write
  fd = open("input.data", O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
  assert( fd>0 && "Couldn't open input data file" );
  data_to_input(fd, (void *)(&data));

  return 0;
}
