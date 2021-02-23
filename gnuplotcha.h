/* ***************************************************************************
 * Copyright © 2013 Raimundo Martins <raimundoomartins@gmail.com>
 * This work is free. It comes withour any warranty, to the extent permitted
 * by applicable law. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
 * ***************************************************************************/

#ifndef __GNUPLOTCHA_H__
#define __GNUPLOTCHA_H__

#include <stdio.h>

enum gpcha_opts { GPCHA_Points = 0, GPCHA_Lines = 1, GPCHA_Vectors = 2, GPCHA_Image = 3,
                  GPCHA_FirstPlot = (1<<4), GPCHA_LastPlot = (1<<5),
                  GPCHA_Array = (1<<6), GPCHA_Record = 0
};

#define GPCHA_PlotTypeMask ((1<<4)-1)
#define GPCHA_PlotOnce (GPCHA_FirstPlot | GPCHA_LastPlot)
#define GPCHA_PlotLimits(plotcount, max) (((!(plotcount)) << 4 /*FirstPlot*/) | (((plotcount) >= (max-1)) << 5 /*LastPlot*/))

FILE *gnuplotcha_open();
void gnuplotcha_mkvid(char *outfile, int vidwidth, int vidheight, float fps,
                      void (*plotsetup)(FILE *gnuplot, void *arg), void *obj1,
                      int (*plotframe)(FILE *gnuplot, int frame, void *arg), void *obj2);
int gnuplotcha_setrange(FILE *gplot, const char * const axis, double min, double max);
int gnuplotcha_plotadd(FILE *gplot, const int Nx, const int Ny, const char *format, const enum gpcha_opts opts, const char *extra, ... );
int gnuplotcha_senddata1d(FILE *gplot, void *data, const size_t Nx, const size_t fieldsize);
int gnuplotcha_senddata1d_stride(FILE *gplot, void *data, const size_t Nx, const size_t offx, const size_t stride, const size_t fieldsize);
int gnuplotcha_senddata2d(FILE *gplot, void **data, const size_t Nx, const size_t Ny, const size_t offx, const size_t fieldsize);

#endif
