/* See LICENSE file for copyright and license details. */

#include "png_io.h"
#include <png.h>
#include <stdlib.h>

png_infop info_ptr;
png_bytepp row_pointers;
png_byte channels;
png_uint_32 width, height;

/* Read a png file */
void read_png(char *filename)
{
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		printf("Failed to open file \"%s\"\n", filename);
		exit(1);
	}
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);  
	png_init_io(png_ptr, fp);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
	row_pointers = png_get_rows(png_ptr, info_ptr);
	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	channels = png_get_channels(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, NULL, NULL); 
	fclose(fp);
}

/* Write a png file */
void write_png(char *filename)
{
	FILE *fp = fopen(filename, "wb");
	if (!fp) {
		printf("Failed to open file \"%s\"\n", filename);
		exit(1);
	}
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_init_io(png_ptr, fp);
	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);
}
