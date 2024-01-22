all:
	cc -g mpl.c parser.c out.c -o mpl
run:
	cc -Wparentheses -DDEBUG=1 mpl.c parser.c out.c -o mpl 
	./mpl tests/new20.c
debug:
	cc -fsanitize=address -DDEBUG=1 -g mpl.c parser.c out.c -o mpl
release:
	cc -g -O3 mpl.c parser.c out.c -o mpl
analyze:
	cc -fanalyzer mpl.c parser.c out.c -o mpl
