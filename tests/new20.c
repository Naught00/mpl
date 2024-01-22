int foo_new(int x, int z) {
	int ret = x * x;
}

int main(int argc) {
	int x;
	x = 16;
	{
		int foo = 90;
		int z;
	}
	int z;
	z = 120;

	z = (3 * 4) + 2;
	x = foo_new(2, 3) + foo_new(4, 5);
}
