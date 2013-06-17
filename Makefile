
GNUPLOTCHAFILES = gnuplotcha.c
LIBS = -lm

all: test snakeplot

test: test.c $(GNUPLOTCHAFILES)
	gcc $(GNUPLOTCHAFILES) $(LIBS) -Wall -o test test.c

snakeplot: snakeplot.c $(GNUPLOTCHAFILES)
	gcc $(GNUPLOTCHAFILES) $(LIBS) -lncurses -Wall -o snakeplot snakeplot.c

clean:
	rm -f test snakeplot
