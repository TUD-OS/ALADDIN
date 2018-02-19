#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "aes.h"

int main(int argc, char **argv) {
  struct bench_args_t data;
  uint8_t initial_contents[DATA_LEN];
  int i, fd;

  fd = open("myinput.dat", O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
  for(i = 0; i < DATA_LEN; ++i)
    initial_contents[i] = i * 2;
  write(fd, initial_contents, DATA_LEN);
  close(fd);

  // Fill data structure
  for(i=0; i<32; i++)
    data.k[i] = i;
  memcpy(data.buf, initial_contents, DATA_LEN);

  // Open and write
  fd = open("input.data", O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
  assert( fd>0 && "Couldn't open input data file" );
  data_to_input(fd, &data);

  return 0;
}
