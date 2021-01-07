/* See LICENSE file for copyright and license details. */

#ifndef PNG_IO_H 
#define PNG_IO_H

#include <png.h>

/* Read a png file */
void read_png(char *filename);

/* Write a png file */
void write_png(char *filename);

#endif /* PNG_IO_H */
