# steg - steganography tool

This is a [steganography][1] tool enabling you to conceal any file
within a PNG file.

## Requirements

In order to build steg, you need [libpng][2].

## Installation

Edit `config.mk` to match your local setup (steg is installed into the
`/usr/local` namespace by default).

Afterward, enter the following command to build and install steg (if
necessary as root):

    make clean install

## Running steg

Hide message: 

    steg -h <file_in> <png_in> <png_out>

Read message: 

    steg -r <png_in> <file_out>

[1]: https://en.wikipedia.org/wiki/Steganography
[2]: http://www.libpng.org/pub/png/libpng.html
