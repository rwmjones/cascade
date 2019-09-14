/* Cascade (C) 1997 Richard W.M. Jones. */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "cascade.h"

void
short_delay (int speed)
{
#if defined(linux) && !defined(__PPC__)
  struct timespec t;

  /* Sleep for approximately 1/20th second. */
  t.tv_sec = 0;
  t.tv_nsec = 5000000 / speed;
  nanosleep (&t, NULL);
#else
  usleep (40000 / speed);
#endif
}
