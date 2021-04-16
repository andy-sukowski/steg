/* See LICENSE file for copyright and license details. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "png_io.h"

/* hide file size in first 32 bits of png */
void hide_fsize(long fsize)
{
	for (short bit_idx = 0; bit_idx < 32; ++bit_idx) {
		int y = bit_idx / (width * channels);
		int x = bit_idx % (width * channels);
		png_byte *value = &rows[y][x];
		*value &= ~1; /* make even */
		if (fsize & 1 << (31 - bit_idx))
			*value |= 1; /* 1 else 0 */
	}
}

/* hide one byte in png */
void hide_byte(unsigned char byte, long byte_idx)
{
	for (short bit_idx = 0; bit_idx < CHAR_BIT; ++bit_idx) {
		long abs_bit_idx = 32 + byte_idx * CHAR_BIT + bit_idx;
		int y = abs_bit_idx / (width * channels);
		int x = abs_bit_idx % (width * channels);
		png_byte *value = &rows[y][x];
		*value &= ~1; /* make even */
		if (byte & 1 << (CHAR_BIT - bit_idx - 1))
			*value |= 1; /* 1 else 0 */
	}
}

/* hide file contents in png */
void hide_file(const char *filename, const char *src_png_name, char *out_png_name)
{
	/* check if reading png was successful */
	if (read_png(src_png_name) == EXIT_FAILURE)
		exit(EXIT_FAILURE);

	/* free allocated memory */
	png_destroy_read_struct(&pngp, NULL, NULL);
	printf("Width: %d\nHeight: %d\nChannels: %d\n", width, height, channels);

	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		fprintf(stderr, "Error: failed to open file '%s'\n", filename);
		exit(EXIT_FAILURE);
	}

	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	printf("File size: %ld\n", fsize);
	hide_fsize(fsize);

	if ((32 + fsize * CHAR_BIT) > (width * height * channels)) {
		fprintf(stderr, "Error: binary file doesn't fit into png file\n");
		fclose(fp);
		exit(EXIT_FAILURE);
	}

	unsigned char content[fsize];
	fread(content, 1, fsize, fp);

	for (long byte_idx = 0; byte_idx < sizeof(content); ++byte_idx)
		hide_byte(content[byte_idx], byte_idx);

	if (write_png(out_png_name) == EXIT_FAILURE)
		exit(EXIT_FAILURE);
	fclose(fp);
}

/* read file size from first 32 bits of png */
long read_fsize(void)
{
	long fsize = 0;
	for (short bit_idx = 0; bit_idx < 32; ++bit_idx) {
		int y = bit_idx / (width * channels);
		int x = bit_idx % (width * channels);
		png_byte value = rows[y][x];
		if (value & 1) /* if odd */
			fsize |= 1 << (31 - bit_idx);
	}
	return fsize;
}

/* read one byte from png */
unsigned char read_byte(long byte_idx)
{
	unsigned char byte = 0;
	for (short bit_idx = 0; bit_idx < CHAR_BIT; ++bit_idx) {
		long abs_bit_idx = 32 + byte_idx * CHAR_BIT + bit_idx;
		int y = abs_bit_idx / (width * channels);
		int x = abs_bit_idx % (width * channels);
		png_byte value = rows[y][x];
		if (value & 1) /* if odd */
			byte |= 1 << (CHAR_BIT - bit_idx - 1);
	}
	return byte;
}

/* read file contents from png */
void read_file(const char *filename, const char *png_name)
{
	/* check if reading png was successful */
	if (read_png(png_name) == EXIT_FAILURE)
		exit(EXIT_FAILURE);
	printf("Width: %d\nHeight: %d\nChannels: %d\n", width, height, channels);

	FILE *fp = fopen(filename, "wb");
	if (!fp) {
		fprintf(stderr, "Error: failed to \nopen file '%s'\n", filename);
		exit(EXIT_FAILURE);
	}

	long fsize = read_fsize();
	printf("File size: %ld\n", fsize);
	unsigned char content[fsize];

	for (long byte_idx = 0; byte_idx < sizeof(content); ++byte_idx)
		content[byte_idx] = read_byte(byte_idx);

	fwrite(content, 1, fsize, fp);
	/* free allocated memory */
	png_destroy_read_struct(&pngp, &infop, NULL);
	fclose(fp);
}

/* handle command line arguments */
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
	return EXIT_SUCCESS;
}
