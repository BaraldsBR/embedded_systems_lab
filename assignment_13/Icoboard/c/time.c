#include "time.h"

#include <fcntl.h>
#include <getopt.h>
#include <linux/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

long time_time(void) {
  struct timeval tv;
  long t;

  gettimeofday(&tv, 0);

  t = ((long)tv.tv_sec * 1E6) + (long)tv.tv_usec;

  return t;
}