run:
	cc mpl.c parser.c out.c
	./a.out new5.txt
debug:
	cc -g mpl.c parser.c
	seergdb ./a.out
release:
	cc -O3 mpl.c parser.c out.c
