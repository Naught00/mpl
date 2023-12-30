run:
	cc mpl.c parser.c out.c -o mpl
	./mpl tests/new9.txt
debug:
	cc -g mpl.c parser.c out.c -o mpl
release:
	cc -O3 mpl.c parser.c out.c -o mpl
analyze:
	cc -fanalyzer mpl.c parser.c out.c -o mpl
