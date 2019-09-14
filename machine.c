/* Cascade (C) 1997 Richard W.M. Jones. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cascade.h"

#ifdef sun
typedef int (*__compar_fn_t)(const void *data1, const void *data2);
#endif /* sun */

#define DOUBLE_NEGATE_BIAS 10
#define NEGATE_BIAS 5
#define IMPOSSIBLE -10000

/* Difficulty level controls. */
static int ply = 1;		/* Number of moves ahead to search. */
static int omit = 3;		/* Number of good moves to omit. */
static int difficulty = 1;	/* Current level of difficulty. */

/* Function prototypes. */
static void search (const state *state_ptr, int depth, int *scores_rtn);

void
set_difficulty (int d)
{
  assert (1 <= d && d <= 5);
  switch (d)
    {
    case 1:
      ply = 1; omit = 3; break;
    case 2:
      ply = 1; omit = 1; break;
    case 3:
      ply = 1; omit = 0; break;

      /* If ply > 1, then omit must be 0. */
    case 4:
      ply = 3; omit = 0; break;
    case 5:
      ply = 5; omit = 0; break;
    }
  difficulty = d;
}

int
get_difficulty (void)
{
  return difficulty;
}

static int
compare_s_and_l_elements (const int *n1, const int *n2)
{
  return *n2 - *n1;
}

/* Given the current state of play "state_ptr", work out a move for
 * the machine to play. Returns the single character to remove.
 */
int
pick_machine_move (const state *state_ptr)
{
  int scores [BD_NR_LETTERS];
  int scores_and_letters [BD_NR_LETTERS][2];
  int i, pick;

  /* Search for the scores from removing each possible letter. */
  search (state_ptr, ply, scores);

  /* Sort 'em. */
  for (i = 0; i < BD_NR_LETTERS; ++i)
    {
      scores_and_letters [i][0] = scores [i];
      scores_and_letters [i][1] = letters [i];
    }
  qsort (scores_and_letters,
	 BD_NR_LETTERS, 2 * sizeof (int),
	 (__compar_fn_t) compare_s_and_l_elements);

  /* Pick the top nth one. */
  pick = omit;
  while (scores_and_letters [pick][0] == IMPOSSIBLE)
    pick --;
  assert (pick >= 0);
  return scores_and_letters [pick][1];
}

/* Search down to depth. */

static void
search (const state *state_ptr, int depth, int *scores_rtn)
{
  int i;

  assert (depth == 1);

  for (i = 0; i < BD_NR_LETTERS; ++i)
    {
      if (! state_ptr->picked [i])
	{
	  int letter = letters [i];
	  state *s = copy_state (state_ptr);

	  remove_letter_from_board (s->board, letter);
	  drop_balls (s->board, s, 1, 0);
	  scores_rtn [i] = s->mscore - s->pscore
	    + (s->negate ?
	       (s->dooble ?
		DOUBLE_NEGATE_BIAS :
		NEGATE_BIAS)
	       : 0);
	  free_state (s);
	}
      else
	scores_rtn [i] = IMPOSSIBLE;
    }
}
