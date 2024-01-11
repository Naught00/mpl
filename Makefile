all:
	cc -g mpl.c parser.c out.c -o mpl
run:
	cc mpl.c parser.c out.c -o mpl
	./mpl tests/new17.txt
debug:
	cc -g mpl.c parser.c out.c -o mpl
release:
	cc -g -O3 mpl.c parser.c out.c -o mpl
analyze:
	cc -fanalyzer mpl.c parser.c out.c -o mpl
