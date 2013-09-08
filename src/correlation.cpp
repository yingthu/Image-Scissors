#include "correlation.h"

/************************ TODO 2 **************************/
/*
 *	INPUT:
 *		origImg:		the original image,
 *		imgWidth:		the width of the image
 *		imgHeight:		the height of the image
 *						the image is arranged such that
 *						origImg[3*(row*imgWidth+column)+0],
 *						origImg[3*(row*imgWidth+column)+1],
 *						origImg[3*(row*imgWidth+column)+2]
 *						are R, G, B values for pixel at (column, row).
 *
 *      kernel:			the 2D filter kernel,
 *		knlWidth:		the width of the kernel
 *		knlHeight:		the height of the kernel
 *
 *		scale, offset:  after correlating the kernel with the origImg,
 *						each pixel should be divided by scale and then added by offset
 *
 *		selection:      a byte array of the same size as the image,
 *						indicating where in the original image should be filtered, e.g.,
 *						selection[k] == 1 ==> pixel k should be filtered
 *                      selection[k] == 0 ==> pixel k should NOT be filtered
 *                      a special case is selection is a NULL pointer, which means all the pixels should be filtered.
 *
 *  OUTPUT:
 *		rsltImg:		the filtered image of the same size as original image.
 *						it is a valid pointer ( allocated already ).
 */
 //Hang's comment: 1)selection is not used
 //                2)length of selection should be equal to length of origImg, rgb considered
 //                3)knlWidth and knlHeight should all be odd numbers
 
 // image_filter & pixel_filter modified by Yingchuan
 // 1) selection applied
 // 2) problem that image_filter didn't work in InitNodeBuf solved
 // 3) selection length is equal to node length (targeting on each pixel)
 // 4) speed up the function

void image_filter(double* rsltImg, const unsigned char* origImg, const unsigned char* selection,
                  int imgWidth, int imgHeight,
                  const double* kernel, int knlWidth, int knlHeight,
                  double scale, double offset)
{
    // Note: copying origImg to rsltImg is NOT the solution, it does nothing!
	/*
	int index;
	double newpixelval;
	int nowcorry,nowcorrx;
	int nowpixelindex,nowpixelvalue;
	double nowkernelvalue;
	for(int j=0;j<imgHeight;j++)
	{
		for(int i=0;i<imgWidth;i++)
		{
			for(int rgb=0;rgb<3;rgb++)
			{
				index=cordinate2idx(i,j,rgb,imgWidth);
				//if(((int)selection[index])==1)
				{
					newpixelval=0;
					for(int jj=0;jj<knlHeight;jj++)
					{
						for(int ii=0;ii<knlWidth;ii++)
						{
							nowcorry=j-(knlHeight-1)/2+jj;
							nowcorrx=i-(knlWidth-1)/2+ii;
							if((nowcorry>=0)&&(nowcorry<imgHeight)&&(nowcorrx>=0)&&(nowcorrx<imgWidth))
							{
								nowkernelvalue=kernel[jj*knlWidth+ii];
								nowpixelindex=cordinate2idx(nowcorrx,nowcorry,rgb,imgWidth);
								nowpixelvalue=(int)origImg[nowpixelindex];
								newpixelval+=(nowpixelvalue*nowkernelvalue);
							}
						}
					}
					rsltImg[index]=newpixelval/scale+offset;
				}
				//else
					//rsltImg[index]=((int)origImg)*1.0;
			}
		}
	}
	*/
	// Added by Yingchuan
	for (int row = 0; row < imgHeight; row++)
		for (int column = 0; column < imgWidth; column++)
		{
			int index = row*imgWidth + column;
			double rgb[3];
			if (selection == NULL || selection[index] ==1)
			{	
				// filter that pixel
				pixel_filter(rgb, column, row, origImg, imgWidth, imgHeight, kernel, knlWidth, knlHeight, scale, offset);
				for (int i = 0; i < 3; i++)
					rsltImg[3*index+i] = rgb[i];
			}
			else
			{
				for (int i = 0; i < 3; i++)
					rsltImg[3*index+i] = origImg[3*index+i];
			}
		}
}
/************************ END OF TODO 2 **************************/


/************************ TODO 3 **************************/
/*
 *	INPUT:
 *      x:				a column index,
 *      y:				a row index,
 *		origImg:		the original image,
 *		imgWidth:		the width of the image
 *		imgHeight:		the height of the image
 *						the image is arranged such that
 *						origImg[3*(row*imgWidth+column)+0],
 *						origImg[3*(row*imgWidth+column)+1],
 *						origImg[3*(row*imgWidth+column)+2]
 *						are R, G, B values for pixel at (column, row).
 *
 *      kernel:			the 2D filter kernel,
 *		knlWidth:		the width of the kernel
 *		knlHeight:		the height of the kernel
 *
 *		scale, offset:  after correlating the kernel with the origImg,
 *						the result pixel should be divided by scale and then added by offset
 *
 *  OUTPUT:
 *		rsltPixel[0], rsltPixel[1], rsltPixel[2]:
 *						the filtered pixel R, G, B values at row y , column x;
 */

void pixel_filter(double rsltPixel[3], int x, int y, const unsigned char* origImg, int imgWidth, int imgHeight,
                  const double* kernel, int knlWidth, int knlHeight,
                  double scale, double offset)
{
	/*
	double newpixelval;
	int nowcorry,nowcorrx;
	int nowpixelindex,nowpixelvalue;
	double nowkernelvalue;
	for(int rgb=0;rgb<3;rgb++)
	{
		newpixelval=0;
		for(int jj=0;jj<knlHeight;jj++)
		{
			for(int ii=0;ii<knlWidth;ii++)
			{
				nowcorry=y-(knlHeight-1)/2+jj;
				nowcorrx=x-(knlWidth-1)/2+ii;
				if((nowcorry>=0)&&(nowcorry<imgHeight)&&(nowcorrx>=0)&&(nowcorrx<imgWidth))
				{
					nowkernelvalue=kernel[jj*knlHeight+ii];
					nowpixelindex=cordinate2idx(nowcorrx,nowcorry,rgb,imgWidth);
					nowpixelvalue=(int)origImg[nowpixelindex];
					newpixelval+=(nowpixelvalue*nowkernelvalue);
				}
			}
		}
		rsltPixel[rgb]=newpixelval/scale+offset;
	}
	*/
	// Added by Yingchuan
	for (int rgb = 0; rgb < 3; rgb++)
	{
		int crossCorr = 0;
		for (int u = -knlWidth/2; u < knlWidth/2; u++)
			for (int v = -knlHeight/2; v < knlWidth/2; v++)
			{
				// break when out of boundary
				if (x+u<0 || x+u>imgWidth || y+v<0 || y+v>imgHeight)
					break;
				// G[x,y] = sum_u{-k}{k}sum_v{-k}{k}F[x+u,y+v]H[u,v]
				// v+knlHeight/2 -- [0,knlHeight]      v+knlWidth/2 -- [0,knlWidth]
				crossCorr += origImg[3*((y+v)*imgWidth+x+u)+rgb] * kernel[(v+knlHeight/2)*knlWidth+u+knlWidth/2];
			}
		rsltPixel[rgb] = crossCorr / scale + offset;
	}
}
/************************ END OF TODO 3 **************************/
/*
int cordinate2idx(int x,int y,int rgb,int imgWidth)
{
	int index;
	index=3*(y*imgWidth+x)+rgb;
	return index;
}
*/