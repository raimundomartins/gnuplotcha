gnuplotcha
==========

Gnulpotcha is a C interface (or C wrapper) to use gnuplot.
It forks a gnuplot process and passes data directly to it,
through pipes, and in binary mode.

It's main goal is to make calling gnuplot from C easier, fast,
and yet transparent and non-intrusive enough to be able to use it
only when needed/wanted.  
It basically has the same purpose as gnuplot_i,
only with binary data, and without temp files.  
It can also compile videos with ffmpeg (through pipes).

Work in progress, but usable enough as it is.
