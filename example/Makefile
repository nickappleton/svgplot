CFLAGS += -I../..

run_example: example
	./example > example.svg

example: example.c ../src/svgplot.c
	$(CC) $(CFLAGS) -o example example.c ../src/svgplot.c
