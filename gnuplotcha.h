/* ***************************************************************************
 * Copyright © 2013 Raimundo Martins <raimundoomartins@gmail.com>
 * This work is free. It comes withour any warranty, to the extent permitted
 * by applicable law. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
 * ***************************************************************************/

#ifndef __GNUPLOTCHA_H

#include <stdio.h>

enum with { GPCHA_Points = 1, GPCHA_Lines, GPCHA_Vectors };
enum last { GPCHA_NotLastPlot = 0, GPCHA_LastPlot = 1 };

FILE *gnuplotcha_open();
void gnuplotcha_mkvid(char *outfile, int vidwidth, int vidheight, float fps,
	void (*plotsetup)(FILE *gnuplot, void *arg), void *obj1,
	int (*plotframe)(FILE *gnuplot, int frame, void *arg), void *obj2);
int gnuplotcha_setrange(FILE *gplot, char axis, double min, double max);
int gnuplotcha_plotadd(FILE *gplot, const int Nx, const int Ny, const char *format, const enum with with, const int last, const char *opts, ...);
int gnuplotcha_senddata1d(FILE *gplot, void *data, const int Nx, const int fieldsize);
int gnuplotcha_senddata2d(FILE *gplot, void **data, const int Nx, const int Ny, const int offx, const int fieldsize);

#endif
