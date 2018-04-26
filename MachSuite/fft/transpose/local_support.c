#include "fft.h"
#include <string.h>

#ifdef GEM5_HARNESS
#include "gem5/gem5_harness.h"
#endif

int INPUT_SIZE = sizeof(struct bench_args_t);

#define EPSILON ((TYPE)1.0e-6)

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
#ifdef GEM5_HARNESS
  mapArrayToAccelerator(
      MACHSUITE_FFT_TRANSPOSE, "in_x", (void*)&args->in_x,
      sizeof(args->in_x));
  mapArrayToAccelerator(
      MACHSUITE_FFT_TRANSPOSE, "in_y", (void*)&args->in_y,
      sizeof(args->in_y));
  mapArrayToAccelerator(
      MACHSUITE_FFT_TRANSPOSE, "out_x", (void*)&args->out_x,
      sizeof(args->out_x));
  mapArrayToAccelerator(
      MACHSUITE_FFT_TRANSPOSE, "out_y", (void*)&args->out_y,
      sizeof(args->out_y));
  invokeAcceleratorAndBlock(MACHSUITE_FFT_TRANSPOSE);
#else
  fft1D_512( args->in_x, args->in_y, args->out_x, args->out_y);
#endif
}

/* Input format:
%% Section 1
TYPE[DATA_LEN]: signal (real part)
%% Section 2
TYPE[DATA_LEN]: signal (complex part)
*/

void input_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->in_x, DATA_LEN);

  s = find_section_start(p,2);
  STAC(parse_,TYPE,_array)(s, data->in_y, DATA_LEN);
}

void data_to_input(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->in_x, DATA_LEN);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->in_y, DATA_LEN);
}

/* Output format:
%% Section 1
TYPE[DATA_LEN]: freq (real part)
%% Section 2
TYPE[DATA_LEN]: freq (complex part)
*/

void output_to_data(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  char *p, *s;
  // Load input string
  p = readfile(fd);

  s = find_section_start(p,1);
  STAC(parse_,TYPE,_array)(s, data->out_x, DATA_LEN);

  s = find_section_start(p,2);
  STAC(parse_,TYPE,_array)(s, data->out_y, DATA_LEN);
}

void data_to_output(int fd, void *vdata) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->out_x, DATA_LEN);

  write_section_header(fd);
  STAC(write_,TYPE,_array)(fd, data->out_y, DATA_LEN);
}

int check_data( void *vdata, void *vref ) {
  struct bench_args_t *data = (struct bench_args_t *)vdata;
  struct bench_args_t *ref = (struct bench_args_t *)vref;
  int has_errors = 0;
  int i;
  double real_diff, img_diff;

  for(i=0; i<DATA_LEN; i++) {
    real_diff = data->out_x[i] - ref->out_x[i];
    img_diff = data->out_y[i] - ref->out_y[i];
    has_errors |= (real_diff<-EPSILON) || (EPSILON<real_diff);
    //if( has_errors )
      //printf("%d (real): %f (%f)\n", i, real_diff, EPSILON);
    has_errors |= (img_diff<-EPSILON) || (EPSILON<img_diff);
    //if( has_errors )
      //printf("%d (img): %f (%f)\n", i, img_diff, EPSILON);
  }

  // Return true if it's correct.
  return !has_errors;
}
