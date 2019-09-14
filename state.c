/* Cascade (C) 1997 Richard W.M. Jones. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <malloc.h>

#include "cascade.h"

state *
init_state (void)
{
  state *s = malloc (sizeof (state));
  if (s == NULL)
    fatal_perror ("malloc");
  memset (s, 0, sizeof (state));
  return s;
}

state *
copy_state (const state *s)
{
  state *copy = malloc (sizeof (state));
  if (s == NULL)
    fatal_perror ("malloc");
  memcpy (copy, s, sizeof (state));
  copy->board = copy_board (s->board);
  return copy;
}

void
free_state (state *s)
{
  free (s);
}

void
generate_board_for_state (state *s)
{
  s->board = init_board ();
  s->balls_in_play = count_balls_on_board (s->board);
}

static inline int
max (int a, int b)
{
  return a > b ? a : b;
}

void
set_score (state *s, int who, int score)
{
  if (s->negate) score = -score;
  if (s->dooble) score *= 2;
  if (who == 0)
    s->pscore = max (s->pscore + score, 0);
  else
    s->mscore = max (s->mscore + score, 0);
}

void
flip_negate (state *s)
{
  s->negate = !s->negate;
}

void
flip_double (state *s)
{
  s->dooble = !s->dooble;
}
