/* Cascade (C) 1997 Richard W.M. Jones. */

#ifndef __cascade_h__
#define __cascade_h__

/* Variables describing the (x,y) location of various things on the
 * screen.
 */
extern int width, height;	/* Screen width, height in chars. */
extern int keys_x, keys_y;	/* Location of key-help banner. */
extern int negf_x, negf_y;	/* Location of NEGATE flag. */
extern int dblf_x, dblf_y;	/* Location of DOUBLE flag. */
extern int pscore_x, pscore_y;	/* Location of player's score. */
extern int mscore_x, mscore_y;	/* Location of machine's score. */
extern int score_width;		/* Width of scores. */
extern int floor_x, floor_y;	/* Location of "floor". */
extern int board_x, board_y;	/* Location of playing board. */
extern int board_width, board_height; /* Size of the playing board. */
extern int roll_y;		/* Line where balls roll along. */
extern int roll_x_min, roll_x_max; /* Stop points for rolling balls. */

/* Board layout. */

#define BD_NR_LETTERS (26+10)

#define BD_EMPTY 0		/* Actually, this has to be zero. */
#define BD_WALL 1		/* Side walls. */
#define BD_BRICK 2		/* Bricks in the middle of the board. */
#define BD_BALL 3		/* Ball. */
#define BD_NEGATE 4		/* ``-''. */
#define BD_DOUBLE 5		/* ``*''. */
#define BD_HEART 6		/* Bonus heart. */

extern char letters [BD_NR_LETTERS];

/* Stuff to maintain the current state of the game. */

struct state {
  int balls_in_play;		/* Balls still on the board. */
  char *board;			/* The board itself. */
  int picked [BD_NR_LETTERS];	/* Flags for letters that are picked. */
  int pscore, mscore;		/* Player score, machine score. */
  int negate, dooble;		/* State of the negate/double flags. */
};

typedef struct state state;

/* Global variable set when "quit" or ^C pressed. */

extern volatile int quit;

/* Function prototypes. */

extern void init_screen (void);
extern void layout_screen (void);
extern void draw_screen (state *);
extern void update_screen (state *);
extern int getkey (void);
extern void clear_screen (void);
extern void clear_line (int);
extern void write_centered (int, const char *);
extern void write_screen (int, int, const char *);
extern void free_screen (void);
extern void rolling_ball_animation (state *, int, int, int);
extern int count_balls_on_board (const char *board);
extern char *init_board (void);
extern state *init_state (void);
extern state *copy_state (const state *);
extern void free_state (state *);
extern void generate_board_for_state (state *);
extern void set_score (state *, int who, int score);
extern void flip_negate (state *);
extern void flip_double (state *);
extern char bd_get (const char *, int, int);
extern void bd_set (char *, int, int, char);
extern char *init_board (void);
extern char *copy_board (const char *);
extern void free_board (char *);
extern int count_balls_on_board (const char *);
extern void remove_letter_from_board (char *, int);
extern void drop_balls (char *, state *, int who_moved, int need_update);
extern void fatal (const char *);
extern void fatal_perror (const char *);
extern void short_delay (int);
extern int pick_machine_move (const state *);
extern void set_difficulty (int);
extern int get_difficulty (void);

#endif /* __cascade_h__ */
