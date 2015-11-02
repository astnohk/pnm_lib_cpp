# pnm_lib_cpp
simple PNM format library


## How to use

It is very simple PNM format library.
It can read/write the PNM (.pbm, .pgm and .ppm) and do some simple image processing (intencity scaling, convert to grayscale, resampling, etc.).

You could read the PNM format with read(const *filename) method.
Of course you could write with write(const *filename) method.
Each method automatically identify the format and if the filename has incorrect extension then correct to right one.

The image data on program is holding in 1-D array format but you could use as if it is 2-D data by using Image(int x, int y) method.

Almost all method is destructive so you should copy the image if you want to convert the image data to another one and keep the original one.
