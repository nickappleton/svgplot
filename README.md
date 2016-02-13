# svgplot

A really disgusting library for graphing data from a C program into an SVG.

I'm really not proud of this code, but the output images are pretty enough. It
was hacked together really quickly to get some visual output of some data and
I've added small bits to it over the years as needed. I've never needed to
generate lots of images, so there are lots of memory leaks... one day, I'll
fix them.

Some of this code was just poked until it happened to do the right thing
(again, I really just wanted something quick to plot things). The code for
the grid axis space iterator is probably much more complicated than it needs
to be.