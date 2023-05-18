run:
	cc mpl.c parser.c
	./a.out new2.txt
debug:
	cc -g mpl.c parser.c
