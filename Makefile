run:
	cc mpl.c parser.c out.c
	./a.out new3.txt
debug:
	cc -g mpl.c parser.c
	seergdb ./a.out
