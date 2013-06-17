/* ***************************************************************************
 * Copyright © 2013 Raimundo Martins <raimundoomartins@gmail.com>
 * This work is free. It comes withour any warranty, to the extent permitted
 * by applicable law. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
 * ***************************************************************************/

#include "gnuplotcha.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

static char * const argsgnuplot[] = { "gnuplot", "-persist", NULL };

FILE *gnuplotcha_open()
{
	int infd_l[2];
	pipe(infd_l);
	if(!fork())
	{
		/* gnuplot */
		close(infd_l[1]);
		dup2(infd_l[0], 0);
		execvp(argsgnuplot[0], argsgnuplot);
		fprintf(stderr, "Failed to exec %s\n", argsgnuplot[0]);
		exit(1);
	}
	return fdopen(infd_l[1], "w");
}

void gnuplotcha_mkvid(char *outfile, int vidwidth, int vidheight, float fps,
		void (*plotsetup)(FILE *gnuplot, void *arg), void *obj1,
		int (*plotframe)(FILE *gnuplot, int frame, void *arg), void *obj2)
{
	char framerate[10], size[43];
	snprintf(framerate, sizeof(framerate), "%.3f", fps);
	snprintf(size, sizeof(size), "%dx%d", vidwidth, vidheight);
	char * const argsffmpeg[] = { "ffmpeg", "-y", "-f", "image2pipe", "-r", framerate, "-vcodec", "png", "-i", "-",
		"-vcodec", "png", "-s", size, "-r", framerate, outfile, NULL };
	int plot_infd[2], ff_infd[2], frame;
	pipe(plot_infd);
	pipe(ff_infd);
	if(!fork())
	{
		/* gnuplot */
		close(plot_infd[1]);
		dup2(plot_infd[0], 0);
		close(ff_infd[0]);
		dup2(ff_infd[1], 1);
		execvp(argsgnuplot[0], argsgnuplot);
		fprintf(stderr, "Failed to exec %s\n", argsgnuplot[0]);
		exit(1);
	}
	close(plot_infd[0]);
	if(!fork())
	{
		/* ffmpeg */
		close(plot_infd[1]);
		close(ff_infd[1]);
		dup2(ff_infd[0], 0);
		execvp(argsffmpeg[0], argsffmpeg);
		fprintf(stderr, "Failed to exec %s\n", argsffmpeg[0]);
		exit(1);
	}
	close(ff_infd[0]);
	close(ff_infd[1]);
	FILE *gnuplot = fdopen(plot_infd[1], "w");
	fprintf(gnuplot, "set t png size %d,%d\n", vidwidth, vidheight);
	if(plotsetup)
		plotsetup(gnuplot, obj1);
	frame = 1;
	while(plotframe(gnuplot, frame++, obj2));
	close(plot_infd[1]);
}

int gnuplotcha_plotadd(FILE *gplot, const int Nx, const int Ny,
		const char * const format, const enum gpcha_opts opts, const char * const extra, ...)
{
	va_list ap;

	fprintf(gplot, "%s '-' binary ", (opts & GPCHA_FirstPlot) ? "plot" : ",");
	if(opts & GPCHA_Array)
	{
		fprintf(gplot, "array=%dx%d format='%s' ",  Nx, Ny, format);
	}
	else
	{
		fprintf(gplot, "record=(%d,%d) format='%s' ",  Nx, Ny, format);
	}
	if(extra)
	{
		va_start(ap, extra);
		vfprintf(gplot, extra, ap);
		va_end(ap);
	}
	switch(opts & GPCHA_PlotTypeMask)
	{
		case GPCHA_Points:  fprintf(gplot, " with points " ); break;
		case GPCHA_Lines:   fprintf(gplot, " with lines "  ); break;
		case GPCHA_Vectors: fprintf(gplot, " with vectors "); break;
		case GPCHA_Image:   fprintf(gplot, " with image "  ); break;
		default:            fprintf(gplot, " with points " );
	}
	if(opts & GPCHA_LastPlot)
	{
		fprintf(gplot, "\n");
	}
	fflush(gplot);
	return 0;
}

int gnuplotcha_setrange(FILE *gplot, const char * const axis, double min, double max)
{
	if(isfinite(min))
	{
		if(isfinite(max)) fprintf(gplot, "set %srange[%lf:%lf]\n", axis, min, max);
		else              fprintf(gplot, "set %srange[%lf:*]\n"  , axis, min);
	}
	else
	{
		if(isfinite(max)) fprintf(gplot, "set %srange[*:%lf]\n", axis, max);
		else              fprintf(gplot, "set %srange[*:*]\n"  , axis);
	}
	fflush(gplot);
	return 0;
}

int gnuplotcha_senddata1d(FILE *gplot, void *data, const size_t Nx, const size_t fieldsize)
{
	return fwrite(data, fieldsize, Nx, gplot);
}

int gnuplotcha_senddata1d_stride(FILE *gplot, void *data, const size_t Nx, const size_t offx, const size_t stride, const size_t fieldsize)
{
	size_t i, count = 0;
	const size_t noffx = offx*fieldsize;
	const size_t nNx = Nx*stride+noffx;
	for(i = noffx; i < nNx; i += stride)
	{
		count += fwrite((char *)data+i, fieldsize, 1, gplot);
	}
	fflush(gplot);
	return count;
}

int gnuplotcha_senddata2d(FILE *gplot, void **data, const size_t Nx, const size_t Ny, const size_t offx, const size_t fieldsize)
{
	size_t j, count = 0;
	const size_t noffx = fieldsize*offx;
	for(j = 0; j < Ny; j++)
	{
		count += fwrite((char *)data[j]+noffx, fieldsize, Nx, gplot);
	}
	fflush(gplot);
	return count;
}
