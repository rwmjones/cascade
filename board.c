/* Cascade (C) 1997 Richard W.M. Jones. */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>

#include "cascade.h"

#define ROWS_OF_BALLS 3 /* Number of balls at top of board to start with. */

char letters [BD_NR_LETTERS] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

inline char
bd_get (const char *board, int x, int y)
{
  assert (board != NULL);
  assert (0 <= x && x < board_width);
  assert (0 <= y && y < board_height);

  return board [x + y * board_width];
}

inline void
bd_set (char *board, int x, int y, char c)
{
  assert (board != NULL);
  assert (0 <= x && x < board_width);
  assert (0 <= y && y < board_height);

  board [x + y * board_width] = c;
}

static void
bricks_at_row (char *board, int y)
{
  int x;

  for (x = 2; x < board_width-2; ++x)
    {
      if (((x-y) % 3) != 0)
	bd_set (board, x, y, BD_BRICK);
    }
}

static void
bricks_along_bottom (char *board)
{
  return bricks_at_row (board, board_height-1);
}

static void
brick_row (char *board, int x, int y, int width)
{
  int i;

  for (i = x-(width-1)/2; i <= x+(width-1)/2; ++i)
    bd_set (board, i, y, BD_BRICK);
}

static void
brick_diamond_at (char *board, int x, int y, int width)
{
  int i;

  for (i = 0; width > 0; width -= 2, i++)
    {
      brick_row (board, x, y-i, width);
      brick_row (board, x, y+i, width);
    }
}

char *
init_board (void)
{
  int i, j, sz;
  char *board = malloc (sz = board_width * board_height * sizeof (char));
  if (board == NULL)
    fatal_perror ("malloc");

  memset (board, BD_EMPTY, sz);

  /* Put in the side walls, which are mandatory. */
  for (j = 0; j < board_height; ++j)
    {
      bd_set (board, 0, j, BD_WALL);
      bd_set (board, board_width-1, j, BD_WALL);
    }

  /* Put random letters in the middle, but leave ROWS_OF_BALLS rows at the top. */
  for (j = ROWS_OF_BALLS; j < board_height; ++j)
    for (i = 1; i < board_width-1; ++i)
      {
	int c = rand () % BD_NR_LETTERS;
	bd_set (board, i, j, letters [c]);
      }

  /* Scatter some doubles, negates and hearts around. */
  for (j = ROWS_OF_BALLS; j < board_height; ++j)
    for (i = 1; i < board_width-1; ++i)
      {
	if ((rand () % 16) == 0)
	  {
	    const char p[] = { BD_HEART, BD_DOUBLE, BD_NEGATE };
	    int c = rand () % 3;
	    bd_set (board, i, j, p[c]);
	  }
      }

  /* Put the balls in at the top. */
  for (j = 0; j < ROWS_OF_BALLS; ++j)
    for (i = 1; i < board_width-1; ++i)
      bd_set (board, i, j, BD_BALL);

  /* Choose a random pattern of bricks. */
  switch (rand () % 4)
    {
    case 0:
      bricks_along_bottom (board);
      brick_diamond_at (board, board_width/2, board_height/2, 11);
      break;
    case 1:
      bricks_along_bottom (board);
      brick_diamond_at (board, board_width/3, board_height/3, 7);
      brick_diamond_at (board, board_width*2/3, board_height/3, 7);
      break;
    case 2:
      brick_diamond_at (board, board_width/3, board_height/3, 7);
      brick_diamond_at (board, board_width*2/3, board_height/3, 7);
      brick_diamond_at (board, board_width/2, board_height*2/3, 7);
      break;
    case 3:
      bricks_along_bottom (board);
      bricks_at_row (board, board_height/5);
      bricks_at_row (board, board_height*2/5);
      bricks_at_row (board, board_height*3/5);
      bricks_at_row (board, board_height*4/5);
      break;
    default:
      assert (0);
    }

  return board;
}

char *
copy_board (const char *board)
{
  int sz;
  char *copy = malloc (sz = board_width * board_height * sizeof (char));
  if (copy == NULL)
    fatal_perror ("malloc");
  memcpy (copy, board, sz);
  return copy;
}

void
free_board (char *board)
{
  free (board);
}

int
count_balls_on_board (const char *board)
{
  int i, j, n = 0;

  for (j = 0; j < board_height; ++j)
    for (i = 0; i < board_width; ++i)
      if (bd_get (board, i, j) == BD_BALL)
	n ++;

  return n;
}

void
remove_letter_from_board (char *board, int letter)
{
  int i, j;

  for (j = 0; j < board_height; ++j)
    for (i = 0; i < board_width; ++i)
      if (bd_get (board, i, j) == letter)
	bd_set (board, i, j, BD_EMPTY);
}

static inline int
is_squashy_item (int c)
{
  return c == BD_EMPTY || c == BD_NEGATE || c == BD_DOUBLE || c == BD_HEART;
}

static void
ball_falls_to_floor (char *board, state *state_ptr, int who_moved,
		     int need_to_update_screen,
		     int old_i, int old_j)
{
  /* Move the ball off the board. */
  bd_set (board, old_i, old_j, BD_EMPTY);

  /* Start the rolling ball animation! */
  if (need_to_update_screen)
    {
      update_screen (state_ptr);
      rolling_ball_animation (state_ptr, old_i, old_j+1, who_moved);
    }

  /* Update the score. */
  set_score  (state_ptr, who_moved, 1);

  /* Update the score on the screen. */
  if (need_to_update_screen)
    update_screen (state_ptr);
}

static void
ball_falls (char *board, state *state_ptr, int who_moved,
	    int need_to_update_screen,
	    int old_i, int old_j,
	    int i, int j, int c)
{
  /* Move the ball. */
  bd_set (board, old_i, old_j, BD_EMPTY);
  bd_set (board, i, j, BD_BALL);

  /* Update the flags and/or score, if appropriate. */
  switch (c)
    {
    case BD_NEGATE:
      flip_negate (state_ptr);
      break;
    case BD_DOUBLE:
      flip_double (state_ptr);
      break;
    case BD_HEART:
      set_score (state_ptr, who_moved, 4);
      break;
    }

  /* Update the screen, if necessary. */
  if (need_to_update_screen)
    {
      update_screen (state_ptr);
      short_delay (1);
    }
}

void
drop_balls (char *board, state *state_ptr,
	    int who_moved,
	    int need_to_update_screen)
{
  int i, j;

  /* FIXME: checks are wrong - need to check sideways space. */
  for (j = board_height-1; j >= 0; --j)
    for (i = 0; i < board_width; ++i)
      if (bd_get (board, i, j) == BD_BALL)
	{
	  int c;

	  /* We have a ball at (i,j). Look below - can it fall? */
	  if (j == board_height-1)
	    {
	      ball_falls_to_floor (board, state_ptr, who_moved,
				   need_to_update_screen, i, j);
	      state_ptr->balls_in_play --;
	    }
	  else if (is_squashy_item (c = bd_get (board, i, j+1)))
	    {
	      ball_falls (board, state_ptr, who_moved,
			  need_to_update_screen, i, j, i, j+1, c);
	      /* Reset iterators, so we catch this ball next time round. */
	      i --;
	      j ++;
	    }
	  else if (is_squashy_item (c = bd_get (board, i-1, j+1)))
	    {
	      ball_falls (board, state_ptr, who_moved,
			  need_to_update_screen, i, j, i-1, j+1, c);
	      i -= 2;
	      j ++;
	    }
	  else if (is_squashy_item (c = bd_get (board, i+1, j+1)))
	    {
	      ball_falls (board, state_ptr, who_moved,
			  need_to_update_screen, i, j, i+1, j+1, c);
	      j ++;
	    }
	}
}
