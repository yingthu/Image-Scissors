#ifndef CONVOLVE_H
#define CONVOLVE_H

#include "imgflt.h"
//#include <iostream>
//using namespace std;

void image_filter(double *rsltImg, const unsigned char *origImg, const unsigned char *selection, 
			int imgWidth, int imgHeight, 
			const double *kernel, int knlWidth, int knlHeight,
			double scale, double offset); 

void pixel_filter(double rsltPixel[3], int x, int y, const unsigned char *origImg, int imgWidth, int imgHeight, 
			const double *kernel, int knlWidth, int knlHeight,
			double scale, double offset);

int cordinate2idx(int x,int y,int rgb,int imgWidth);
#endif