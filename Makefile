all:
	cc -g mpl.c newparser.c out.c -o mpl
run:
	cc mpl.c newparser.c out.c -o mpl
	./mpl tests/new14.txt
debug:
	cc -g mpl.c parser.c out.c -o mpl
release:
	cc -g -O3 mpl.c newparser.c out.c -o mpl
analyze:
	cc -fanalyzer mpl.c parser.c out.c -o mpl
