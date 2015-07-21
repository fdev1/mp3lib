#include <stdio.h>

int main(int argc, char* argv[])
{

	FILE* fi;
	FILE* fo;
	unsigned char b;

	fi = fopen(argv[1], "rb");
	fo = fopen(argv[2], "wb");

	while (fread(&b, 1, 1, fi))
	{
		fprintf(fo, "%02x\n", b);
	}

	fclose(fo);
	fclose(fi);

	return 0;


}
