#include <stdio.h>
#include <stdlib.h>

static unsigned char gethex(const char *s, char **endptr) {
  	return strtoul(s, endptr, 16);
}

int main(int argc, char* argv[])
{

	FILE* fi;
	FILE* fo;
	unsigned char bb;
	unsigned char b[3];

	fi = fopen(argv[1], "r");
	fo = fopen(argv[2], "wb");

	while (fread(&b[0], 1, 1, fi))
	{
		fread(&b[1], 1, 1, fi);
		fread(&b[2], 1, 1, fi);
		b[2] = 0;

		bb = (unsigned char) strtoul(b, NULL, 16);
	

		fwrite(&bb, 1, 1, fo);
	}

	fclose(fo);
	fclose(fi);

	return 0;


}
