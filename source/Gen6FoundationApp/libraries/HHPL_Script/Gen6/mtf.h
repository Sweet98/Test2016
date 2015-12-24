#ifndef _MTF_H_
#define _MTF_H_

/*
 * image_addr:   image buffer address
 * image_width:  the pixel number of image width
 * image_height: the pixel number of image height
 * x1:           coordinate start of X
 * x2:           coordinate end of X
 * y1:           coordinate start of Y
 * y2:           coordinate end of Y
 * hor:          calculating mtf along horizon or column
 * mtf_value:    return the mtf value
 * return@       if success, return 0; otherwise < 0
 */
extern int lib_mtf(unsigned char *image_addr,
		   int image_width,
		   int image_height,
		   int x1,
		   int x2,
		   int y1,
		   int y2,
		   int horizon,
		   float *mtf_value);

#endif

