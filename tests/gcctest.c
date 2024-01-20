#include <stdio.h>

struct test {
	int  x;
	long y;
};

struct test2 {
	int  x;
	int  y;
};

int test_function(int x, int z, int y, int a, int b
		, int c, int d, int e, int f, int g,
		int h, int i) {
	int m = 10;

	{
		char afs[256];
		int fooo;
		float nooo;
		fooo = 10;
		nooo = 5.0;
	}
	m = i;
	return x * x;
}

int oog_function(int z, int f, int g) {
	int x = 10;
	if (x) {
		return 0;
	} else {
		return 1;
	}
}

int struct_function(struct test z) {
	return 0;
}

int struct_function2(struct test2 z) {
	return 0;
}


int main(void) {
	int i, x, z;
	i = 10;
	x = -2;
	z = i * x;

	struct test newtest;
	newtest.x = 10;
	newtest.y = 100;

	struct test2 est;
	est.x = 10;
	est.y = 100;
	struct_function2(est);

	long y = 90;
	long w = y * y;

	char c[256];
	for (i = 0; i < 256; i++)
		c[i] = 'f';

	int ret;
	ret = test_function(i, i, i, i, i, i, i, i, i, i, i, i);

	oog_function(i, i, i);
	printf("test\n");
	return ret;
}


