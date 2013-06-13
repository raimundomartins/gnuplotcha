/* ***************************************************************************
 * Copyright © 2013 Raimundo Martins <raimundoomartins@gmail.com>
 * This work is free. It comes withour any warranty, to the extent permitted
 * by applicable law. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
 * ***************************************************************************/

#include <stdlib.h>
#include "gnuplotcha.h"

int main(int argc, char **argv) {
	int i, j;
	double data[] = { 1, 1, 2, 2 };
	double **data2 = malloc(sizeof(double *)*4);
	for(j = 0; j < 4; j++) {
		data2[j] = malloc(sizeof(double) * 2 * 3);
		for(i = 0; i < 3; i++)
		{
			data2[j][i*2]   = 1;/* vec i, j, x component */
			data2[j][i*2+1] = 2;/* vec i, j, y component */
		}
	}

	FILE *gp = gnuplotcha_open();

	gnuplotcha_setrange(gp, 'x', -1, 12);
	gnuplotcha_setrange(gp, 'y', -1, 12);

	fprintf(gp, "set title 'can still send stuff like this'\n");

	gnuplotcha_plotadd(gp, 2, 1, "%lf%lf", GPCHA_Points | GPCHA_FirstPlot, "using 1:($2*%lf) title '%d Points (y scaled by %.2lf)'", 5.0, 2, 5.0);
	gnuplotcha_plotadd(gp, 3, 4, "%lf%lf", GPCHA_Vectors | GPCHA_LastPlot, "using 0:-1:1:2 title 'Vectors'");

	gnuplotcha_senddata1d(gp, data, 2, sizeof(double)*2);
	gnuplotcha_senddata2d(gp, (void **)data2, 3, 4, 0, sizeof(double)*2);

	return 0;
}
