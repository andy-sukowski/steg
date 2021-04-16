/* See LICENSE file for copyright and license details. */

#include <png.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "png_io.h"

#define PNG_BYTES_TO_CHECK 4

/* global variables */
png_struct *pngp;
png_info *infop;
png_byte **rows;
png_uint_32 width, height;
png_byte channels;

/* read png file */
int read_png(const char *filename)
{
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		fprintf(stderr, "Error: failed to open file '%s'\n", filename);
		return EXIT_FAILURE;
	}

	/* read signature bytes */
	unsigned char sig[PNG_BYTES_TO_CHECK];
	if (fread(sig, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK) {
		fclose(fp);
		fprintf(stderr, "Error: failed to read signature bytes"
				"from '%s'\n", filename);
		return EXIT_FAILURE;
	}

	/* compare first bytes of signature */
	if (png_sig_cmp(sig, 0, PNG_BYTES_TO_CHECK)) {
		fclose(fp);
		fprintf(stderr, "Error: '%s' is not a PNG file\n", filename);
		return EXIT_FAILURE;
	}

	/* initialize png_struct `pngp` */
	pngp = png_create_read_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);
	if (!pngp) {
		fclose(fp);
		fprintf(stderr, "Error: memory allocation failed\n");
		return EXIT_FAILURE;
	}

	/* allocate memory for image information */
	infop = png_create_info_struct(pngp);  
	if (!infop) {
		fclose(fp);
		png_destroy_read_struct(&pngp, &infop, NULL);
		return EXIT_FAILURE;
	}

	/* set error handling */
	if (setjmp(png_jmpbuf(pngp))) {
		fclose(fp);
		png_destroy_read_struct(&pngp, &infop, NULL);
		return EXIT_FAILURE;
	}

	/* set up output control */
	png_init_io(pngp, fp);

	/* because we read some of the signature */
	png_set_sig_bytes(pngp, PNG_BYTES_TO_CHECK);

	/* read entire image into info structure */
	png_read_png(pngp, infop, PNG_TRANSFORM_IDENTITY, NULL);

	/* optain information */
	rows = png_get_rows(pngp, infop);
	width = png_get_image_width(pngp, infop);
	height = png_get_image_height(pngp, infop);
	channels = png_get_channels(pngp, infop);

	/*
	 * Allocated memory has to be freed outside this function like this:
	 * png_destroy_read_struct(&pngp, &infop, NULL);
	 */
	fclose(fp);
	return EXIT_SUCCESS;
}

/* write png file */
int write_png(const char *filename)
{
	FILE *fp = fopen(filename, "wb");
	if (!fp) {
		fprintf(stderr, "Error: failed to open file '%s'\n", filename);
		return EXIT_FAILURE;
	}

	/* initialize png_struct `pngp` */
	pngp = png_create_write_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);
	if (!pngp) {
		fclose(fp);
		fprintf(stderr, "Error: memory allocation failed\n");
		return EXIT_FAILURE;
	}

	/* set up input control */
	png_init_io(pngp, fp);

	/* save new pixel values */
	png_set_rows(pngp, infop, rows);

	/* all image data in info structure */
	png_write_png(pngp, infop, PNG_TRANSFORM_IDENTITY, NULL);

	/* free allocated memory */
	png_destroy_write_struct(&pngp, &infop);
	fclose(fp);
	return EXIT_SUCCESS;
}
