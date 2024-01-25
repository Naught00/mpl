int foo_new(int x, int z) {
	int ret = x * x;
	return ret;
}

int exit(int x);

int main(int argc) {
	int x;
	x = 16;
	{
		int foo = 90;
		int z;
		int ll;
		int ll2;
		int ll24;
	}
	int z;
	z = 120;

	z = (3 * 4) + 2;
	x = 2 + foo_new(2, 7);
	z = 99;

	exit(x);
	return z;
}
