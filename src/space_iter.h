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

#ifndef SPACE_ITER_H
#define SPACE_ITER_H

#include <math.h>

struct space_iterator;

/* Create an iterator suiable for generating grid-paper style spacings with
 * both major and minor line categories. The spacings are either both linear
 * or the majors can be logarithmic with linear minor spacings (specified by
 * the is_log boolean). The start and end data points are specified by the
 * start and end parameters. The iterator may not necessarily generate points
 * on these values unless they touch where an expected line would be. */
static
void
space_iterator_init
	(struct space_iterator *iter
	,int                    is_log
	,double                 start
	,double                 end
	,double                 base_interval
	,unsigned               sub_divisions
	);

/* Reset the iterator to the initial point ready for use again. */
static
void
space_iterator_reset
	(struct space_iterator *iter
	);

/* Get the next point. The function returns true if values were obtained. If
 * the function returns false, the iteration is complete and the results will
 * be unspecified. */
static
int
space_iterator_next
	(struct space_iterator *iter
	,double *result
	,int *is_major
	);

/* DO NOT touch the members of this structure directly. Always use the API
 * functions. */
struct space_iterator {
	/* iterator configuration */
	double   start;
	double   end;
	double   base_interval;
	int      is_log;
	unsigned sub_divisions;

	/* iterator data */
	unsigned minor_idx;
	int      major_idx;
};

static
int
space_iterator_next
	(struct space_iterator *iter
	,double *result
	,int *is_major
	)
{
	double pos;

	if (iter->is_log) {
		double rstart = pow(iter->base_interval, iter->major_idx);
		double rend = rstart * iter->base_interval;
		pos = rstart + iter->minor_idx * (rend - rstart) / iter->sub_divisions;
	} else {
		pos = (iter->major_idx + iter->minor_idx / (double)iter->sub_divisions) * iter->base_interval;
	}

	if (pos > iter->end)
		return 0;

	if (is_major != NULL)
		*is_major = (iter->minor_idx == 0);

	if (result != NULL)
		*result = pos;

	iter->minor_idx = (iter->minor_idx + 1) % iter->sub_divisions;
	if (iter->minor_idx == 0)
		iter->major_idx++;

	return 1;
}

static
void
space_iterator_reset
	(struct space_iterator *iter
	)
{
	if (iter->is_log) {
		double initial_range;
		iter->major_idx = floor(log(iter->start) / log(iter->base_interval));
		initial_range = pow(iter->base_interval, iter->major_idx);
		iter->minor_idx = ceil((iter->start - initial_range) * iter->sub_divisions / (initial_range * iter->base_interval - initial_range));
		if (iter->minor_idx >= iter->sub_divisions) {
			iter->major_idx++;
			iter->minor_idx = 0;
		}
	} else {
		iter->major_idx = floor(iter->start / iter->base_interval);
		iter->minor_idx = ceil((iter->start - iter->base_interval * iter->major_idx) * iter->sub_divisions / iter->base_interval);
	}
}

static
void
space_iterator_init
	(struct space_iterator *iter
	,int                    is_log
	,double                 start
	,double                 end
	,double                 base_interval
	,unsigned               sub_divisions
	)
{
	iter->is_log        = is_log;
	iter->sub_divisions = sub_divisions;
	iter->start         = start;
	iter->end           = end;
	iter->base_interval = base_interval;
	space_iterator_reset(iter);
}

#endif /* SPACE_ITER_H */
