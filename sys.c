/* Cascade (C) 1997 Richard W.M. Jones. */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "cascade.h"

void
short_delay (int speed)
{
  struct timespec t;

  t.tv_sec = 0;
  t.tv_nsec = 10 * 1000000 / speed;
  nanosleep (&t, NULL);
}
