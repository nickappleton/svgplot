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

#ifndef SVGPLOT_H
#define SVGPLOT_H

#include <stdio.h>

struct data_set;

struct svgplot {
    double x_min;
    double x_max;
    double y_min;
    double y_max;

    struct data_set *first;
};

struct svgplot_gridaxisinfo {
    /* Non-zero when the axis should be drawn */
    int      is_visible;

    /* For a linear plot, major gridlines will appear at data points that are
     * integer multiples of this value. For a logarithmic plot, major
     * gridlines will appear at integer powers of this value. */
    double   major_interval;

    /* The number of minor divisions which will appear between major data
     * points. This value includes the major divisor so a value of one
     * indicates that there are no minor division lines. The value must be
     * greater than 0. */
    unsigned sub_divisions;

    /* Show text alongside the axis */
    int      show_text;

    /* Non-zero if the axis should be plotted logarithmically. */
    int      is_log;

    /* Non-zero if the plotting region should be automatically sized from the
     * supplied data sets. */
    int      auto_size;

    /* These have no meaning if auto_size is non-zero. Otherwise, they specify
     * the extremities of the graph region. */
    double   start;
    double   end;
};

struct svgplot_gridinfo {
    struct svgplot_gridaxisinfo x;
    struct svgplot_gridaxisinfo y;
};

/* Initialise a new plot object. */
void svgplot_create(struct svgplot *plot);

/* Add a data-set to the plot. If everything goes well, the function returns
 * zero. Otherwise, we ran out of memory. :( */
int
svgplot_add_data
    (struct svgplot            *plot
    ,const double              *x_points
    ,const double              *y_points
    ,unsigned                   num_points
    );

/* Generates the plots. The SVG text is written into dest (which should be
 * opened in text-mode. The SVG object is undefined after this call. */
int
svgplot_finalise
    (struct svgplot                *plot
    ,const struct svgplot_gridinfo *gridinfo
    ,double                         width_cm
    ,double                         height_cm
    ,double                         border_cm
    ,FILE                          *dest
    );

#endif /* SVGPLOT_H */
