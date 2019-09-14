/* Cascade (C) 1997 Richard W.M. Jones. */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#ifdef HAVE_NCURSES
#include <ncurses.h>
#else
#include <curses.h>
#endif
#include <assert.h>

#include "cascade.h"

/* Code to handle the display. */

int width, height;		/* Screen width, height in chars. */
int keys_x, keys_y;		/* Location of key-help banner. */
int negf_x, negf_y;		/* Location of NEGATE flag. */
int dblf_x, dblf_y;		/* Location of DOUBLE flag. */
int pscore_x, pscore_y;		/* Location of player's score. */
int mscore_x, mscore_y;		/* Location of machine's score. */
int score_width;		/* Width of scores. */
int floor_x, floor_y;		/* Location of "floor". */
int board_x, board_y;		/* Location of playing board. */
int board_width, board_height;	/* Size of the playing board. */
int roll_y;			/* Line where balls roll along. */
int roll_x_min, roll_x_max;	/* Stop points for rolling balls. */

/* Curses tags for various attributes. */
#define REVERSE A_REVERSE
#define BOLD A_BOLD

void
layout_screen (void)
{
  /* Keyboard banner goes at the top of the screen. */
  keys_y = 0; keys_x = 0;

  /* Floor at the bottom of the screen. */
  floor_y = height-1; floor_x = 0;

  /* Somewhere for the balls to roll. */
  roll_y = floor_y-1;
  roll_x_min = 5;
  roll_x_max = width-1-5;

  /* Player/machine scores just above the floor. */
  score_width = 4;
  pscore_y = mscore_y = floor_y-1;
  pscore_x = 0;
  mscore_x = width-score_width;

  /* Place negate & double flags at the left side of the screen, just
   * above the player's score.
   */
  negf_y = pscore_y - 3; negf_x = 0;
  dblf_y = pscore_y - 5; dblf_x = negf_x;

  /* Decide on the size of the board. */
  board_width = width - 20;
  board_height = height - 5;

  /* Put the board in the centre of the screen. */
  board_y = 2;
  board_x = (width - board_width)/2;
}

static char *
string (int c, int n)
{
  static char *s = NULL;
  int i;

  if (s != NULL)
    free (s);

  s = malloc ((n+1) * sizeof (char));
  if (s == NULL) fatal_perror ("malloc");

  for (i = 0; i < n; ++i)
    s [i] = c;
  s [n] = 0;

  return s;
}

void
draw_screen (state *s)
{
  /* Clear the screen. */
  clear ();

  attron (BOLD);

  /* Draw the floor. */
  mvaddstr (floor_y, floor_x, string ('=', width));

  /* Draw the keys. */
  mvaddstr (keys_y, keys_x, "Keys: ...");

  attroff (BOLD);

  /* Draw the rest of the stuff. */
  update_screen (s);
}

static inline void
display_board_char (char c)
{
  switch (c)
    {
    case BD_EMPTY: addch (' ');
      break;
    case BD_WALL:  attron (REVERSE); addch ('|'); attroff (REVERSE);
      break;
    case BD_BRICK: attron (REVERSE); addch (' '); attroff (REVERSE);
      break;
    case BD_BALL:  attron (BOLD); addch ('o'); attroff (BOLD);
      break;
    case BD_NEGATE:attron (BOLD); addch ('-'); attroff (BOLD);
      break;
    case BD_DOUBLE:attron (BOLD); addch ('*'); attroff (BOLD);
      break;
    case BD_HEART: attron (BOLD); addch ('$'); attroff (BOLD);
      break;
    default:
      addch (c);
    }
}

void
update_screen (state *s)
{
  char temp [16];
  int i, j;

  attron (BOLD);

  /* Draw the player/machine scores. */
  sprintf (temp, "%04d", s->pscore);
  mvaddstr (pscore_y, pscore_x, temp);
  sprintf (temp, "%04d", s->mscore);
  mvaddstr (mscore_y, mscore_x, temp);

  /* Draw the negate & double flags. */
  if (s->negate)
    mvaddstr (negf_y, negf_x, "- NEGATE");
  else
    mvaddstr (negf_y, negf_x, "        ");
  if (s->dooble)
    mvaddstr (dblf_y, dblf_x, "* DOUBLE");
  else
    mvaddstr (dblf_y, dblf_x, "        ");

  attroff (BOLD);

  /* Draw the board. */
  for (j = 0; j < board_height; ++j)
    {
      move (board_y+j, board_x);
      for (i = 0; i < board_width; ++i)
	display_board_char (bd_get (s->board, i, j));
    }

  /* Update the physical terminal. */
  setsyx (0, 0);
  refresh ();
}

/* More low-level screen drawing routines. */

int
getkey (void)
{
  int c, got_key = 0;

  while (!got_key)
    {
      c = getch ();
      if (c == 'l' - 'a' + 1) /* ie. ^L - redraw screen */
	redrawwin (stdscr);
      else if (c == KEY_BREAK) /* ie. ^C - quit */
	{
	  quit = 1;
	  c = EOF;
	  got_key = 1;
	}
      else
	got_key = 1;
    }

  return c;
}

void
clear_screen (void)
{
  clear ();
}

void
write_centered (int y, const char *s)
{
  mvaddstr (y, (width - strlen (s)) / 2, s);
}

void
write_screen (int y, int x, const char *s)
{
  mvaddstr (y, x, s);
}

void
clear_line (int y)
{
  mvaddstr (y, 0, string (' ', width));
}

/* Initialize the screen. */

void
init_screen (void)
{
  initscr ();
  cbreak ();
  noecho ();
  nonl ();
  intrflush (stdscr, FALSE);
  keypad (stdscr, TRUE);
  curs_set (0);

  /* Check that screen starts at (0,0) */
  {
    int x, y;
    getbegyx (stdscr, y, x);
    assert (x == 0 && y == 0);
  }

  /* Get the width & height of the screen. */
  getmaxyx (stdscr, height, width);

  if (height < 24 || width < 60)
    fatal ("screen or window not large enough to play game");
}

void
free_screen (void)
{
  endwin ();
}

/* This is a convenient place to do the rolling ball animation! */

static inline void
draw_ball (int i, int j)
{
  attron (BOLD);
  mvaddch (j, i, 'o');
  attroff (BOLD);
}

static inline void
undraw_ball (int i, int j)
{
  mvaddch (j, i, ' ');
}

static inline void
refresh_and_wait (int speed)
{
  setsyx (0, 0);
  refresh ();
  short_delay (speed);
}

void
rolling_ball_animation (state *state_ptr, int i, int j, int who_moved)
{
  i += board_x;
  j += board_y;

  while (j <= roll_y)
    {
      draw_ball (i, j);
      refresh_and_wait (1);
      undraw_ball (i, j);
      j ++;
    }
  j --;

  if (who_moved == 0)		/* player */
    {
      while (i >= roll_x_min)
	{
	  i --;
	  draw_ball (i, j);
	  refresh_and_wait (2);
	  undraw_ball (i, j);
	}
    }
  else				/* machine */
    {
      while (i <= roll_x_max)
	{
	  i ++;
	  draw_ball (i, j);
	  refresh_and_wait (2);
	  undraw_ball (i, j);
	}
    }

  refresh ();
}
