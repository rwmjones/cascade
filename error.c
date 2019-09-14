/* Cascade (C) 1997 Richard W.M. Jones. */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>

#include "cascade.h"

void
fatal (const char *msg)
{
  free_screen ();
  fprintf (stderr, "cascade: %s\n", msg);
  exit (1);
}

void
fatal_perror (const char *msg)
{
  free_screen ();
  perror (msg);
  exit (1);
}
