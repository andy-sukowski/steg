/* See LICENSE file for copyright and license details. */

#ifndef PNG_IO_H 
#define PNG_IO_H

#include <png.h>

/* global variables */
extern png_struct *pngp;
extern png_info *infop;
extern png_byte **rows;
extern png_byte channels;
extern png_uint_32 width, height;

/* read png file */
int read_png(const char *filename);

/* write png file */
int write_png(const char *filename);

#endif /* PNG_IO_H */
