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

FILE *gnuplotcha_open() {
	char * const args[] = { "gnuplot", "-persist", NULL };
	int infd_l[2];
	pipe(infd_l);
	if(!fork()) {
		/* gnuplot */
		close(infd_l[1]);
		dup2(infd_l[0], 0);
		execvp(argsgnuplot[0], argsgnuplot);
		fprintf(stderr, "Failed to exec %s\n", args[0]);
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
	if(!fork()) {
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
	if(!fork()) {
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
	const char *format, const enum with with, const int last, const char *opts, ...)
{
	static int plot_count = 0;
	va_list ap;

	if(!gplot || !format || Nx < 0 || Ny < 0)
		return -1;
	fprintf(gplot, "%s '-' binary record=(%d,%d) format='%s' ", plot_count ? "," : "plot",  Nx, Ny, format);
	if(opts) {
		va_start(ap, opts);
		vfprintf(gplot, opts, ap);
		va_end(ap);
	}
	switch(with) {
		case GPCHA_Points:
			fprintf(gplot, " with points ");
			break;
		case GPCHA_Lines:
			fprintf(gplot, " with lines ");
			break;
		case GPCHA_Vectors:
			fprintf(gplot, " with vectors ");
			break;
		default:
			fprintf(gplot, " with points ");
	}
	if(last) {
		plot_count = 0;
		fprintf(gplot, "\n");
	}
	else
		plot_count++;
	fflush(gplot);
	return plot_count;
}

int gnuplotcha_setrange(FILE *gplot, char axis, double min, double max) {
	if(isfinite(min)) {
		if(isfinite(max)) fprintf(gplot, "set %crange[%lf:%lf]\n", axis, min, max);
		else fprintf(gplot, "set %crange[%lf:*]\n", axis, min);
	}
	else {
		if(isfinite(max)) fprintf(gplot, "set %crange[*:%lf]\n", axis, max);
		else fprintf(gplot, "set %crange[*:*]\n", axis);
	}
	fflush(gplot);
	return 0;
}

int gnuplotcha_senddata1d(FILE *gplot, void *data, const int Nx, const int fieldsize) {
	return write(fileno(gplot), data, fieldsize*Nx);
}

int gnuplotcha_senddata2d(FILE *gplot, void **data, const int Nx, const int Ny, const int offx, const int fieldsize) {
	int j, count = 0;
	int fplot = fileno(gplot);
	int offsetx = fieldsize*offx;
	int size = fieldsize*Nx;
	for(j = 0; j < Ny; j++)
		count += write(fplot, data[j] + offsetx, size);
	fflush(gplot);
	return count;
}
