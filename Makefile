
GNUPLOTCHAFILES = gnuplotcha.c
LIBS = -lm

all: test snakeplot

test:
	gcc $(GNUPLOTCHAFILES) $(LIBS) -Wall -o test test.c

snakeplot:
	gcc $(GNUPLOTCHAFILES) $(LIBS) -lncurses -Wall -o snakeplot snakeplot.c

clean:
	rm -f test snakeplot
