/* See LICENSE file for copyright and license details. */

#include "png_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern png_bytepp row_pointers;
extern png_byte channels;
extern png_uint_32 width, height;

/* Hide one byte in the png */
void hide_byte(unsigned char byte, long i)
{
	short b;
	for (b = 0; b < CHAR_BIT; b++) {
		long ib = i * CHAR_BIT + b;
		int y = ib / channels / width;
		int x = ib % (width * channels);
		png_byte *value = &row_pointers[y][x];
		if (byte & (1 << (CHAR_BIT - b - 1)))
			*value = *value / 2 * 2 + 1;
		else
			*value = *value / 2 * 2;
	}
}

/* Hide file contents in the png */
void hide_file(char *filename, char *src_png_name, char *out_png_name)
{
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		printf("Failed to open file \"%s\"\n", filename);
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	unsigned char buffer[4 + fsize];
	buffer[0] = fsize >> 24;
	buffer[1] = fsize >> 16;
	buffer[2] = fsize >> 8;
	buffer[3] = fsize;

	fread(&buffer[4], fsize, 1, fp);
	printf("sizeof(buffer) = %ld\n", sizeof(buffer));

	read_png(src_png_name);

	printf("width: %d | height: %d | channels: %d\n", width, height, channels);

	if ((CHAR_BIT * sizeof(buffer)) > (width * height * channels)) {
		printf("The binary file doesn't fit into the png file.\n");
		exit(1);
	}

	long i;
	for (i = 0; i < sizeof(buffer); i++) {
		hide_byte(buffer[i], i);
	}

	write_png(out_png_name);
	fclose(fp);
}

/* Read one byte from the png */
void read_byte(unsigned char *byte, long i)
{
	short b;
	for (b = 0; b < CHAR_BIT; b++) {
		long ib = i * CHAR_BIT + b;
		int y = ib / channels / width;
		int x = ib % (width * channels);
		png_byte *value = &row_pointers[y][x];
		if (*value % 2 == 1)
			*byte += 1 << (7 - ib % CHAR_BIT);
	}
}

/* Read file contents from the png */
void read_file(char *filename, char *png_name)
{
	FILE *fp = fopen(filename, "wb");
	if (!fp) {
		printf("Failed to open file \"%s\"", filename);
		exit(1);
	}

	read_png(png_name);

	long fsize = 0;
	short ib;
	for (ib = 0; ib < 32; ib++) {
		int y = ib / (width * channels);
		int x = ib % (width * channels);
		png_byte *value = &row_pointers[y][x];
		if (*value % 2 == 1)
			fsize += 1 << (31 - ib);
	}

	unsigned char buffer[4 + fsize];
	memset(buffer, 0, sizeof(buffer));
	printf("sizeof(buffer) = %ld\n", sizeof(buffer));

	long i;
	for (i = 0; i < sizeof(buffer); i++) {
		read_byte(&buffer[i], i);
	}

	fwrite(&buffer[4], fsize, 1, fp);
	fclose(fp);
}

/* Handle command line arguments */
int main(int argc, char *argv[])
{
	if (argc == 5 && !strcmp(argv[1], "-h")) {
		hide_file(argv[2], argv[3], argv[4]);
	} else if (argc == 4 && !strcmp(argv[1], "-r")) {
		read_file(argv[3], argv[2]);
	} else {
		printf("Hide message: %s -h <file_in> <png_in> <png_out>\n"
			"Read message: %s -r <png_in> <file_out>\n",
			argv[0], argv[0]);
	}
	return 0;
}
