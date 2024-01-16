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

int main(void) {
	int i, x, z;
	i = 10;
	x = -2;
	z = i * x;

	long y = 90;
	long w = y * y;

	char c[256];
	for (i = 0; i < 256; i++)
		c[i] = 'f';

	//test_function(i, i, i, i, i);
	printf("test\n");
	return 0;
}


int oog_function() {
	int x = 10;
	if (x) {
		return 0;
	} else {
		return 1;
	}
}

