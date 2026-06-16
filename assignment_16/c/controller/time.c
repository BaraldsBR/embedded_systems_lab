#include "time.h"

#include <fcntl.h>
#include <getopt.h>
#include <linux/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>


long time_time(void) {
  struct timeval tv;
  long t;

  gettimeofday(&tv, 0);

  t = ((long)tv.tv_sec * 1E6) + (long)tv.tv_usec;

  return t;
}

// adapted from https://blat-blatnik.github.io/computerBear/making-accurate-sleep-function/
void precise_sleep(int usec) {
  static double estimate = 250;
  static double mean = 250;
  static double m2 = 0;
  static int64_t count = 1;

  while (usec > estimate) {
    long start = time_time();
    usleep(100);
    long end = time_time();

    int observed = end - start;
    usec -= observed;

    ++count;
    double delta = observed - mean;
    mean += delta / count;
    m2   += delta * (observed - mean);
    double stddev = sqrt(m2 / (count - 1));
    estimate = mean + stddev;
  }

  // spin lock
  long start = time_time();
  while ((time_time() - start) < usec);

}