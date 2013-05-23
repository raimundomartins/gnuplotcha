
GNUPLOTCHAFILES = gnuplotcha.c
LIBS = -lm

test:
	gcc $(GNUPLOTCHAFILES) $(LIBS) -Wall -o test test.c

clean:
	rm test gnuplotcha.o
