/* Copyright (c) 2016 Nick Appleton
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE. */

#include "svgplot/svgplot.h"
#include <math.h>

int main(int argc, char *argv[])
{
	double data_set_x[1024];
	double data_set_y[1024];
	struct svgplot plot;
	struct svgplot_gridinfo gi;
	unsigned i;

	svgplot_create(&plot);

	for (i = 0; i < 1024; i++) {
		data_set_x[i] = i;
		data_set_y[i] = cos(2 * M_PI * i / 333.0);
	}
	(void)svgplot_add_data(&plot, data_set_x, data_set_y, 1024);

	for (i = 0; i < 1024; i++) {
		data_set_x[i] = i;
		data_set_y[i] = sin(2 * M_PI * i / 333.0);
	}
	(void)svgplot_add_data(&plot, data_set_x, data_set_y, 1024);

	gi.x.is_log = 0;
	gi.x.is_visible = 1;
	gi.x.major_interval = 100;
	gi.x.sub_divisions = 10;
	gi.x.show_text = 1;
	gi.x.auto_size = 1;
	gi.y.is_log = 0;
	gi.y.is_visible = 1;
	gi.y.major_interval = 1;
	gi.y.sub_divisions = 5;
	gi.y.show_text = 1;
	gi.y.auto_size = 1;

	svgplot_finalise
		(&plot
		,&gi
		,20
		,12
		,0.2
		,stdout
		,1
		);
}
