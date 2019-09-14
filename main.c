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

volatile int quit = 0;

static void catch_quit (int);
static void main_menu (void);
static void play_game (void);
static void play_single_move (int);
static int  player_moves (void);
static int  machine_moves (void);
static void play_letter (int, int);
static int  letter_ok_and_not_picked (state *, int);
static void picked_letter (state *, int);
static void connect_dialog (void);
static void end_of_game_dialog (void);

int
main (void)
{
  /* Initialize PRNG. */
  srand (time (NULL));

  /* Initialize ncurses screen library. */
  init_screen ();

  /* Make sure various signals are caught and handled gracefully. */
  signal (SIGINT, catch_quit);
  signal (SIGTERM, catch_quit);
  signal (SIGQUIT, catch_quit);
  /* Ignore SIGPIPE, SIGHUP, as these indicate we have lost the
   * terminal anyway.
   */

  /* Main loop. Let's play. */
  while (!quit)
    main_menu ();

  /* Clean up & quit. */
  free_screen ();
  exit (0);
}

static void
catch_quit (int sig)
{
  quit = 1;
}

/*----------------------------------------------------------------------*/

static void
main_menu (void)
{
  int c;
  char buffer [80];

  clear_screen ();

  write_centered (4, "CASCADE (C) 1997 Richard W.M. Jones.");
  write_screen (6, 10,  "'n'       New game against the machine");
  write_screen (8, 10,  "'c'       Connect to another Internet player");
  write_screen (10, 10, "'1' - '5' Select difficulty level, where 1 is easy and");
  write_screen (11, 10, "          5 is bastard mode");
  write_screen (13, 10, "'q'       Quit game");

  sprintf (buffer, "Difficulty: %d", get_difficulty ());
  write_centered (15, buffer);

 again:
  switch (c = getkey ())
    {
    case 'n': case 'N':
      play_game ();
      break;
    case 'c': case 'C':
      connect_dialog ();
      break;
    case '1': case '2': case '3': case '4': case '5':
      set_difficulty (c - '0');
      break;
    case 'q': case 'Q':
      quit = 1;
      break;
    default:
      goto again;
    }
}

static void
connect_dialog (void)
{
}

/*----------------------------------------------------------------------*/

/* State of the current game. */
static state *theState;

static void
play_game (void)
{
  int who_moves = 0;

  layout_screen ();

  theState = init_state ();
  generate_board_for_state (theState);

  draw_screen (theState);

  /* Loop through player's and machine's goes. */
  while (!quit && theState->balls_in_play > 0)
    {
      play_single_move (who_moves);
      who_moves = !who_moves;
    }

  if (!quit)
    end_of_game_dialog ();
}

static void
play_single_move (int who_moves)
{
  int letter;

  if (who_moves == 0)
    letter = player_moves ();
  else
    letter = machine_moves ();

  if (quit) return;

  picked_letter (theState, letter);

  play_letter (who_moves, letter);
}

static int
player_moves (void)
{
  int letter;

  do {
    letter = toupper (getkey ());
  } while (!quit &&
	   !letter_ok_and_not_picked (theState, letter));

  return letter;
}

static int
machine_moves (void)
{
  return pick_machine_move (theState);
}

static int
letter_ok_and_not_picked (state *s, int letter)
{
  const char *t = strchr (letters, letter);
  int i;
  if (t == NULL)
    return 0;			/* Not a letter in the array. */
  i = t - letters;		/* Get offset into this array. */
  if (s->picked [i])
    return 0;			/* It's been picked already. */
  return 1;
}

static void
picked_letter (state *s, int letter)
{
  const char *t = strchr (letters, letter);
  int i;
  assert (t != NULL);
  i = t - letters;
  assert (!s->picked [i]);
  s->picked [i] = 1;
}

/* Play the letter that was picked by the player or machine, updating
 * the screen appropriately, and updating the current state of play.
 */
static void
play_letter (int who_moved, int letter)
{
  /* Remove all instances of this letter from the board. */
  remove_letter_from_board (theState->board, letter);
  update_screen (theState);

  /* Let the balls fall. */
  drop_balls (theState->board, theState, who_moved, 1);
}

static void
end_of_game_dialog (void)
{
  int player_margin = theState->pscore - theState->mscore;

  clear_line (0);
  clear_line (1);

  if (player_margin >= 10)
    write_centered (0, "You thrashed ma ass, boy !");
  else if (player_margin >= 5)
    write_centered (0, "Good show, old chap !");
  else if (player_margin > 0)
    write_centered (0, "I'll beat you next time !");
  else if (player_margin == 0)
    write_centered (0, "Oooh ... dead heat !");
  else if (player_margin >= -5)
    write_centered (0, "Bad luck ... I beat you, but only just !");
  else if (player_margin >= -10)
    write_centered (0, "You can do better than that !");
  else
    write_centered (0, "Ah ha! Victory is mine !");

  write_centered (1, "Press any key to go back to the menu");

  getkey ();
}
