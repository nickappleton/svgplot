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

/* TODO: fix the memory leaks. */

#include "svgplot/svgplot.h"
#include "space_iter.h"
#include <math.h>
#include <stdlib.h>

struct data_set {
	double          *x_points;
	double          *y_points;
	unsigned         nb_points;
	struct data_set *next;
};

void svgplot_create(struct svgplot *plot)
{
	plot->first = NULL;
}

int
svgplot_add_data
	(struct svgplot            *plot
	,const double              *x_points
	,const double              *y_points
	,unsigned                   num_points
	)
{
	struct data_set *ds = malloc(sizeof(*ds));
	if (ds != NULL) {
		unsigned i;
		ds->x_points = malloc(sizeof(double) * num_points);
		ds->y_points = malloc(sizeof(double) * num_points);
		ds->nb_points = num_points;
		if (plot->first == NULL) {
			plot->x_min = x_points[0];
			plot->x_max = plot->x_min;
			plot->y_min = y_points[0];
			plot->y_max = plot->y_min;
		}
		for (i = 0; i < num_points; i++) {
			double x = x_points[i];
			double y = y_points[i];
			plot->x_min = fmin(plot->x_min, x);
			plot->x_max = fmax(plot->x_max, x);
			plot->y_min = fmin(plot->y_min, y);
			plot->y_max = fmax(plot->y_max, y);
			ds->x_points[i] = x;
			ds->y_points[i] = y;
		}
		ds->next = plot->first;
		plot->first = ds;
	}

	return 0;
}

#define PLOT_MINORS (0)
#define PLOT_MAJORS (1)
#define PLOT_TEXT   (2)

void
render_gridline_set
	(FILE  *dest
	,double data_left
	,double data_top
	,double data_right
	,double data_bottom
	,int    is_log
	,int    mode
	,int    is_x
	,double start
	,double end
	,double major_interval
	,unsigned spacings
	)
{
	struct space_iterator iter;
	double res;
	int is_major;

	space_iterator_init(&iter, is_log, start, end, major_interval, spacings);
	while (space_iterator_next(&iter, &res, &is_major)) {
		double warped_pos =
			(   is_log
			?   ((log(res) - log(start)) / (log(end) - log(start)))
			:   (res - start) / (end - start)
			);
		double x1, y1, x2, y2;
		if (is_x) {
			x1 = data_left + warped_pos * (data_right - data_left);
			x2 = x1;
			y1 = data_top;
			y2 = data_bottom;
		} else {
			x1 = data_left;
			x2 = data_right;
			y1 = data_bottom - warped_pos * (data_bottom - data_top);
			y2 = y1;
		}
		if ((mode == PLOT_TEXT) && (is_major)) {
			double tx, ty;
			if (is_x) {
				tx = x1 + 0;
				ty = y2 + 25;
			} else {
				tx = x1 - 70;
				ty = y2 + 7;
			}
			fprintf(dest, "<text x=\"%f\" y=\"%f\" fill=\"black\" font-family=\"Verdana\" font-size=\"24\">%g</text>\n", tx, ty, res);
		} else if (((mode == PLOT_MINORS) && (!is_major)) || ((mode == PLOT_MAJORS) && (is_major))) {
			fprintf(dest, "M %f %f L %f %f ", x1, y1, x2, y2);
		}
	}
}

int
svgplot_finalise
    (struct svgplot                *plot
    ,const struct svgplot_gridinfo *gridinfo
    ,double                         width_cm
    ,double                         height_cm
    ,double                         border_cm
    ,FILE                          *dest
    ,int                            write_svg_header
    )
{
	double data_left = border_cm * 100;
	double data_right = width_cm * 100 - border_cm * 100;
	double data_top = border_cm * 100;
	double data_bottom = height_cm * 100 - border_cm * 100;
	double start_x = (gridinfo->x.auto_size) ? plot->x_min : gridinfo->x.start;
	double end_x   = (gridinfo->x.auto_size) ? plot->x_max : gridinfo->x.end;
	double start_y = (gridinfo->y.auto_size) ? plot->y_min : gridinfo->y.start;
	double end_y   = (gridinfo->y.auto_size) ? plot->y_max : gridinfo->y.end;
	struct data_set *ds = plot->first;

	unsigned colidx = 0;
	const char *colours[] = {"red", "blue", "green", "black"};

	if (write_svg_header) {
		fprintf
			(dest
			,"<?xml version=\"1.0\" standalone=\"yes\"?>\n"
			 "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"
			);
	}

	fprintf
		(dest
		,"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%lfcm\" height=\"%lfcm\" viewBox=\"0 0 %lf %lf\" version=\"1.1\" fill=\"black\">\n"
		,width_cm
		,height_cm
		,width_cm * 100
		,height_cm * 100
		);

	/* Draw a grid */
	if (gridinfo->x.is_visible || gridinfo->y.is_visible) {
		if (gridinfo->x.is_visible && gridinfo->x.show_text)
			data_left += 80;

		if (gridinfo->y.is_visible && gridinfo->y.show_text)
			data_bottom -= 30;

		/* Draw minor grid lines */
		fprintf(dest, "<path d=\"");
		if (gridinfo->x.is_visible) {
			render_gridline_set
				(dest
				,data_left
				,data_top
				,data_right
				,data_bottom
				,gridinfo->x.is_log
				,PLOT_MINORS
				,1
				,start_x
				,end_x
				,gridinfo->x.major_interval
				,gridinfo->x.sub_divisions
				);
		}
		if (gridinfo->y.is_visible) {
			render_gridline_set
				(dest
				,data_left
				,data_top
				,data_right
				,data_bottom
				,gridinfo->y.is_log
				,PLOT_MINORS
				,0
				,start_y
				,end_y
				,gridinfo->y.major_interval
				,gridinfo->y.sub_divisions
				);
		}
		fprintf(dest, "\" stroke=\"lightgrey\" stroke-width=\"2\" fill=\"none\" />");

		fprintf(dest, "<path d=\"");
		if (gridinfo->x.is_visible) {
			render_gridline_set
				(dest
				,data_left
				,data_top
				,data_right
				,data_bottom
				,gridinfo->x.is_log
				,PLOT_MAJORS
				,1
				,start_x
				,end_x
				,gridinfo->x.major_interval
				,gridinfo->x.sub_divisions
				);
		}
		if (gridinfo->y.is_visible) {
			render_gridline_set
				(dest
				,data_left
				,data_top
				,data_right
				,data_bottom
				,gridinfo->y.is_log
				,PLOT_MAJORS
				,0
				,start_y
				,end_y
				,gridinfo->y.major_interval
				,gridinfo->y.sub_divisions
				);
		}
		fprintf(dest, "\" stroke=\"grey\" stroke-width=\"3\" fill=\"none\" />");

		if (gridinfo->x.is_visible) {
			render_gridline_set
				(dest
				,data_left
				,data_top
				,data_right
				,data_bottom
				,gridinfo->x.is_log
				,PLOT_TEXT
				,1
				,start_x
				,end_x
				,gridinfo->x.major_interval
				,gridinfo->x.sub_divisions
				);
		}
		if (gridinfo->y.is_visible) {
			render_gridline_set
				(dest
				,data_left
				,data_top
				,data_right
				,data_bottom
				,gridinfo->y.is_log
				,PLOT_TEXT
				,0
				,start_y
				,end_y
				,gridinfo->y.major_interval
				,gridinfo->y.sub_divisions
				);
		}
	}

	while (ds) {
		unsigned i;
		static const unsigned area_points = 800;

		if (ds->nb_points > area_points) {
			double min_points[800];
			double max_points[800];
			double x_points[800];
			const double segment_width = 1.0 / area_points;
			unsigned nb_points = 0;

			fprintf(dest, "<path d=\"");
			i = 0;
			while (i < ds->nb_points) {
				double min_y = ds->y_points[i];
				double max_y;
				double first_x = ds->x_points[i];
				double last_x;

				if (gridinfo->y.is_log)
					min_y = (log(min_y) - log(start_y)) / (log(end_y) - log(start_y));
				else
					min_y = (min_y - start_y) / (end_y - start_y);

				max_y = min_y;

				if (gridinfo->x.is_log)
					first_x = (log(first_x) - log(start_x)) / (log(end_x) - log(start_x));
				else
					first_x = (first_x - start_x) / (end_x - start_x);

				last_x = first_x;

				while (++i < ds->nb_points) {
					double this_x = ds->x_points[i];
					double this_y = ds->y_points[i];
					if (gridinfo->x.is_log)
						this_x = (log(this_x) - log(start_x)) / (log(end_x) - log(start_x));
					else
						this_x = (this_x - start_x) / (end_x - start_x);

					if (this_x > first_x + segment_width)
						break;

					last_x = this_x;

					if (gridinfo->y.is_log)
						this_y = (log(this_y) - log(start_y)) / (log(end_y) - log(start_y));
					else
						this_y = (this_y - start_y) / (end_y - start_y);

					min_y = fmin(min_y, this_y);
					max_y = fmax(max_y, this_y);
				}

				min_points[nb_points] = min_y;
				max_points[nb_points] = max_y;
				x_points[nb_points++] = (first_x + last_x) * 0.5;
			}

			for (i = 0; i < nb_points; i++) {
				double x = x_points[i];
				double y = max_points[i];

				y = fmin(y, 1.0);
				y = fmax(y, 0.0);

				x = data_left + x * (data_right - data_left);
				y = data_bottom - y * (data_bottom - data_top) - 0.01;

				if (i == 0)
					fprintf(dest, "M %f %f ", x, y);
				else
					fprintf(dest, "L %f %f ", x, y);
			}

			for (i = 0; i < nb_points; i++) {
				double x = x_points[nb_points-i-1];
				double y = min_points[nb_points-i-1];
				x = data_left + x * (data_right - data_left);

				y = fmin(y, 1.0);
				y = fmax(y, 0.0);

				y = data_bottom - y * (data_bottom - data_top) + 0.01;
				fprintf(dest, "L %f %f ", x, y);
			}

			fprintf(dest, "Z\" stroke=\"%s\" stroke-width=\"4\" fill=\"%s\" />", colours[colidx], colours[colidx]);
		} else {
			fprintf(dest, "<path d=\"");
			for (i = 0; i < ds->nb_points; i++) {
				double x = ds->x_points[i];
				double y = ds->y_points[i];
				double warped_x, warped_y;
				if (gridinfo->x.is_log)
					warped_x = (log(x) - log(start_x)) / (log(end_x) - log(start_x));
				else
					warped_x = (x - start_x) / (end_x - start_x);
				if (gridinfo->y.is_log)
					warped_y = (log(y) - log(start_y)) / (log(end_y) - log(start_y));
				else
					warped_y = (y - start_y) / (end_y - start_y);

				x = data_left + warped_x * (data_right - data_left);
				y = data_bottom - warped_y * (data_bottom - data_top);
				if (i == 0)
					fprintf(dest, "M %f %f ", x, y);
				else
					fprintf(dest, "L %f %f ", x, y);
			}
			fprintf(dest, "\" stroke=\"%s\" stroke-width=\"5\" fill=\"none\" />", colours[colidx]);
		}

		colidx = (colidx + 1) % (sizeof(colours) / sizeof(colours[0]));
		ds = ds->next;
	}

	/* Draw the enclosing box */
	fprintf
		(dest
		,"  <rect fill=\"none\" x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" stroke=\"black\" stroke-width=\"3\" />\n"
		,data_left
		,data_top
		,data_right - data_left
		,data_bottom - data_top
		);

	fprintf
		(dest
		,"</svg>\n"
		);

	return 0;
}

