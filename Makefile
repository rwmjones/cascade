# Define features:
#	HAVE_NCURSES	if you have <ncurses.h>, else uses <curses.h>

DEFINES		= -DHAVE_NCURSES -I/usr/include/ncurses

# Libraries:
#	$(NCURSES_LIB)	if you have ncurses
#	$(CURSES_LIB)	if you have ordinary curses

LIBS		= $(NCURSES_LIB)

#----------------------------------------------------------------------

CC		= gcc
CFLAGS		= -O2 -Wall $(DEFINES)

OBJS		= board.o error.o machine.o main.o screen.o state.o sys.o

NCURSES_LIB	= -lncurses
CURSES_LIB	= -lcurses -ltermcap

all:		cascade

clean:
		rm -f $(OBJS) cascade *~ *.bak core

cascade:	$(OBJS)
		$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

.c.o:
		$(CC) $(CFLAGS) -c $< -o $@

$(OBJS):	cascade.h
