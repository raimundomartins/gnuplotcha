
GNUPLOTCHAFILES = gnuplotcha.c
LIBS = -lm

test:
	gcc $(GNUPLOTCHAFILES) $(LIBS) -Wall -o test test.c

snakeplot
	gcc $(GNUPLOTCHAFILES) $(LIBS) -lncurses -Wall -o snakeplot snakeplot.c

clean:
	rm test snakeplot gnuplotcha.o
