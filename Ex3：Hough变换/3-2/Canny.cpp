#include <iostream>
#include "Canny.h"

using namespace std;

/* The constructor. */
Canny::Canny(char *in, float s, float tl, float th, char *dir) {
	if(VERBOSE) cout << "Reading the image " << in << "." << endl;
	infilename = in;
	src.load(infilename);
	sigma = s;
	tlow = tl;
	thigh = th;
	dirfilename = dir;
	w = src._width;
	h = src._height;
	image.assign(w, h, 1, 1, 0);
	smoothedim.assign(w, h, 1, 1, 0);
	delta_x.assign(w, h, 1, 1, 0);
	delta_y.assign(w, h, 1, 1, 0);
	magnitude.assign(w, h, 1, 1, 0);
	nms.assign(w, h, 1, 1, 0);
	edge.assign(w, h, 1, 1, 0);
	
	if (dirfilename != NULL) {
		sprintf(composedfname, "%s_s_%3.2f_l_%3.2f_h_%3.2f.fim", infilename,
		sigma, tlow, thigh);
		dirfilename = composedfname;
   }
}

/* Start the edge detection process. */
CImg<unsigned char> Canny::edge_detection() {
	if(VERBOSE) cout << "Starting Canny edge detection." << endl;

	/****************************************************************************
	* Convert the image to grey scale.
	****************************************************************************/
	if(VERBOSE) cout << "Converting the image to grey scale." << endl;
	to_grey_scale();

	/****************************************************************************
	* Perform gaussian smoothing on the image using the input standard
	* deviation.
	****************************************************************************/
	if(VERBOSE) cout << "Smoothing the image using a gaussian kernel." << endl;
    gaussian_smooth();

	/****************************************************************************
	* Compute the first derivative in the x and y directions.
	****************************************************************************/
    if(VERBOSE) cout << "Computing the X and Y first derivatives." << endl;
    derrivative_x_y();

	/****************************************************************************
	* This option to write out the direction of the edge gradient was added
	* to make the information available for computing an edge quality figure
	* of merit.
	****************************************************************************/
    if (dirfilename != NULL) {
		/*************************************************************************
		* Compute the direction up the gradient, in radians that are
		* specified counteclockwise from the positive x-axis.
		*************************************************************************/
    	radian_direction();

		/*************************************************************************
		* Write the gradient direction image out to a file.
		*************************************************************************/
		if((fpdir = fopen(dirfilename, "wb")) == NULL) {
			fprintf(stderr, "Error opening the file %s for writing.\n", dirfilename);
			exit(1);
		}
		fwrite(dir_radians, sizeof(float), h * w, fpdir);
		fclose(fpdir);
		free(dir_radians);
    }

	/****************************************************************************
	* Compute the magnitude of the gradient.
	****************************************************************************/
    if(VERBOSE) cout << "Computing the magnitude of the gradient." << endl;
    magnitude_x_y();

	/****************************************************************************
	* Perform non-maximal suppression.
	****************************************************************************/
    if(VERBOSE) cout << "Doing the non-maximal suppression." << endl;
    non_max_supp();

	/****************************************************************************
	* Use hysteresis to mark the edge pixels.
	****************************************************************************/
    if(VERBOSE) cout << "Doing hysteresis thresholding." << endl;
    apply_hysteresis();

	/****************************************************************************
	* Delete edges that are shorter than 20 px.
	****************************************************************************/
    if(VERBOSE) cout << "Deleting short edges." << endl;
    optimize();
    
	/****************************************************************************
	* Write out the edge image to a file.
	****************************************************************************/
    output();
    return edge;
}

/*******************************************************************************
* PROCEDURE: to_grey_scale
* PURPOSE: Convert the image to grey scale.
*******************************************************************************/
void Canny::to_grey_scale() {
	cimg_forXY(src, x, y) {
		int r = src(x, y, 0);
		int g = src(x, y, 1);
		int b = src(x, y, 2);
		int grey = (r * 30 + g * 59 + b * 11 + 50) / 100;
		image(x, y) = grey;
	}
}

/*******************************************************************************
* PROCEDURE: gaussian_smooth
* PURPOSE: Blur an image with a gaussian filter.
*******************************************************************************/
void Canny::gaussian_smooth() {
	int rr, cc,     	/* Counter variables. */
	windowsize,         /* Dimension of the gaussian kernel. */
	center;             /* Half of the windowsize. */
	float *tempim,      /* Buffer for separable filter gaussian smoothing. */
		  *kernel,      /* A one dimensional gaussian kernel. */
	      dot,          /* Dot product summing variable. */
	      sum;          /* Sum of the kernel weights variable. */

	/****************************************************************************
	* Create a 1-dimensional gaussian smoothing kernel.
	****************************************************************************/
	if (VERBOSE) cout << "   Computing the gaussian smoothing kernel." << endl;
	make_gaussian_kernel(&kernel, &windowsize);
	center = windowsize / 2;


	/****************************************************************************
	* Allocate a temporary buffer image and the smoothed image.
	****************************************************************************/
	if ((tempim = (float *) calloc(w * h, sizeof(float))) == NULL) {
		fprintf(stderr, "Error allocating the buffer image.\n");
		exit(1);
	}

	/****************************************************************************
	* Blur in the x - direction.
	****************************************************************************/
	if (VERBOSE) cout << "   Bluring the image in the X-direction." << endl;
	cimg_forXY(image, x, y) {
		dot = 0.0;
		sum = 0.0;
		for (cc = (-center); cc <= center; cc++) {
			if (((x + cc) >= 0) && ((x + cc) < w)) {
				dot += (float)image(x + cc, y) * kernel[center + cc];
				sum += kernel[center + cc];
			}
		}
		tempim[y * w + x] = dot / sum;
	}

	/****************************************************************************
	* Blur in the y - direction.
	****************************************************************************/
	if(VERBOSE) cout << "   Bluring the image in the Y-direction." << endl;
	cimg_forXY(image, x, y) {
		dot = 0.0;
		sum = 0.0;
		for (rr = (-center); rr <= center; rr++) {
			if (((y + rr) >= 0) && ((y + rr) < h)) {
				dot += tempim[(y + rr) * w + x] * kernel[center + rr];
				sum += kernel[center + rr];
			}
		}
		smoothedim(x, y) = (short int)(dot * BOOSTBLURFACTOR / sum + 0.5);
	}
	free(tempim);
	free(kernel);
}

/*******************************************************************************
* PROCEDURE: derrivative_x_y
* PURPOSE: Compute the first derivative of the image in both the x any y
* directions. The differential filters that are used are:
*
*                                          -1
*         dx =  -1 0 +1     and       dy =  0
*                                          +1
*
*******************************************************************************/
void Canny::derrivative_x_y() {
	/****************************************************************************
	* Compute the x-derivative. Adjust the derivative at the borders to avoid
	* losing pixels.
	****************************************************************************/
	if(VERBOSE) cout << "   Computing the X-direction derivative." << endl;
	cimg_forXY(smoothedim, x, y) {
		if (x == 0) {
			delta_x(x, y) = smoothedim(x + 1, y) - smoothedim(x, y);
		}
		else if (x == w - 1) {
			delta_x(x, y) = smoothedim(x, y) - smoothedim(x - 1, y);
		}
		else {
			delta_x(x, y) = smoothedim(x + 1, y) - smoothedim(x - 1, y);
		}
	}

	/****************************************************************************
	* Compute the y-derivative. Adjust the derivative at the borders to avoid
	* losing pixels.
	****************************************************************************/
	if(VERBOSE) cout << "   Computing the Y-direction derivative." << endl;
	cimg_forXY(smoothedim, x, y) {
		if (y == 0) {
			delta_y(x, y) = smoothedim(x, y + 1) - smoothedim(x, y);
		}
		else if (y == h - 1) {
			delta_y(x, y) = smoothedim(x, y) - smoothedim(x, y - 1);
		}
		else {
			delta_y(x, y) = smoothedim(x, y + 1) - smoothedim(x, y - 1);
		}
	}
}

/*******************************************************************************
* Procedure: radian_direction
* Purpose: To compute a direction of the gradient image from component dx and
* dy images. Because not all derriviatives are computed in the same way, this
* code allows for dx or dy to have been calculated in different ways.
*
* The resulting angle is in radians measured counterclockwise from the
* xdirection. The angle points "up the gradient".
*******************************************************************************/
void Canny::radian_direction() {
	int r, c, pos;
	double dx, dy;

	/****************************************************************************
	* Allocate an image to store the direction of the gradient.
	****************************************************************************/
	if ((dir_radians = (float *) calloc(w * h, sizeof(float))) == NULL) {
		fprintf(stderr, "Error allocating the gradient direction image.\n");
		exit(1);
	}

	cimg_forXY(delta_x, x, y) {
		dx = (double)delta_x(x, y);
		dy = -(double)delta_y(x, y);
		dir_radians[y * w + x] = (float)angle_radians(dx, dy);
	}
}

/*******************************************************************************
* PROCEDURE: magnitude_x_y
* PURPOSE: Compute the magnitude of the gradient. This is the square root of
* the sum of the squared derivative values.
*******************************************************************************/
void Canny::magnitude_x_y() {
	int sq1, sq2;
	cimg_forXY(magnitude, x, y) {
		sq1 = pow((int)delta_x(x, y), 2);
		sq2 = pow((int)delta_y(x, y), 2);
		magnitude(x, y) = (short int)(0.5 + sqrt((float)sq1 + (float)sq2));
	}
}

/*******************************************************************************
* PROCEDURE: non_max_supp
* PURPOSE: This routine applies non-maximal suppression to the magnitude of
* the gradient image.
*******************************************************************************/
void Canny::non_max_supp() {
    short z1, z2;
    short m00, gx, gy;
    float mag1, mag2, xperp, yperp;


	/****************************************************************************
	* Zero the edges of the nms image.
	****************************************************************************/
	cimg_forXY(nms, x, y) {
		if (x == 0 || x == w - 1 || y == 0 || y == h - 1) {
			nms(x, y) = 0;
		}
	}

	/****************************************************************************
	* Suppress non-maximum points.
	****************************************************************************/
	cimg_forXY(nms, x, y) {
		if (x != 0 && x != w - 1 && y != 0 && y != h - 1) {
			m00 = magnitude(x, y);
			if (m00 == 0) {
				nms(x, y) = (unsigned char) NOEDGE;
			}
			else {
				gx = delta_x(x, y);
				gy = delta_y(x, y);
				xperp = -gx / ((float)m00);
				yperp = gy / ((float)m00);
			}

			if (gx >= 0) {
				if (gy >= 0) {
					if (gx >= gy) {
						/* 111 */
						/* Left point */
						z1 = magnitude(x - 1, y);
						z2 = magnitude(x - 1, y - 1);

						mag1 = (m00 - z1) * xperp + (z2 - z1) * yperp;

						/* Right point */
						z1 = magnitude(x + 1, y);
						z2 = magnitude(x + 1, y + 1);

						mag2 = (m00 - z1) * xperp + (z2 - z1) * yperp;
					}
					else {
						/* 110 */
						/* Left point */
						z1 = magnitude(x, y - 1);
						z2 = magnitude(x - 1, y - 1);

						mag1 = (z1 - z2) * xperp + (z1 - m00) * yperp;

						/* Right point */
						z1 = magnitude(x, y + 1);
						z2 = magnitude(x + 1, y + 1);

						mag2 = (z1 - z2) * xperp + (z1 - m00) * yperp;
					}
				}
				else {
					if (gx >= -gy) {
			            /* 101 */
			            /* Left point */
			            z1 = magnitude(x - 1, y);
			            z2 = magnitude(x - 1, y + 1);

			            mag1 = (m00 - z1) * xperp + (z1 - z2) * yperp;

			            /* Right point */
			            z1 = magnitude(x + 1, y);
			            z2 = magnitude(x + 1, y - 1);

			            mag2 = (m00 - z1) * xperp + (z1 - z2) * yperp;
			        }
			        else {
			            /* 100 */
			            /* Left point */
			            z1 = magnitude(x, y + 1);
			            z2 = magnitude(x - 1, y + 1);

			            mag1 = (z1 - z2) * xperp + (m00 - z1) * yperp;

			            /* Right point */
			            z1 = magnitude(x, y - 1);
			            z2 = magnitude(x + 1, y - 1);

			            mag2 = (z1 - z2) * xperp  + (m00 - z1) * yperp;
			        }
				}
			}
			else {
				if (gy >= 0) {
					if (-gx >= gy) {
						/* 011 */
			            /* Left point */
			            z1 = magnitude(x + 1, y);
			            z2 = magnitude(x + 1, y - 1);

			            mag1 = (z1 - m00) * xperp + (z2 - z1) * yperp;

			            /* Right point */
			            z1 = magnitude(x - 1, y);
			            z2 = magnitude(x - 1, y + 1);

			            mag2 = (z1 - m00) * xperp + (z2 - z1) * yperp;
					}
					else {
						/* 010 */
			            /* Left point */
			            z1 = magnitude(x, y - 1);
			            z2 = magnitude(x + 1, y - 1);

			            mag1 = (z2 - z1) * xperp + (z1 - m00) * yperp;

			            /* Right point */
			            z1 = magnitude(x, y + 1);
			            z2 = magnitude(x - 1, y + 1);

			            mag2 = (z2 - z1) * xperp + (z1 - m00) * yperp;
					}
				}
				else {
					if (-gx > -gy) {
						/* 001 */
			            /* Left point */
			            z1 = magnitude(x + 1, y);
			            z2 = magnitude(x + 1, y + 1);

			            mag1 = (z1 - m00) * xperp + (z1 - z2) * yperp;

			            /* Right point */
			            z1 = magnitude(x - 1, y);
			            z2 = magnitude(x - 1, y - 1);

			            mag2 = (z1 - m00) * xperp + (z1 - z2) * yperp;
					}
					else {
						/* 000 */
			            /* Left point */
			            z1 = magnitude(x, y + 1);
			            z2 = magnitude(x + 1, y + 1);

			            mag1 = (z2 - z1) * xperp + (m00 - z1) * yperp;

			            /* Right point */
			            z1 = magnitude(x, y - 1);
			            z2 = magnitude(x - 1, y - 1);

			            mag2 = (z2 - z1) * xperp + (m00 - z1) * yperp;
					}
				}
			}

			/* Now determine if the current point is a maximum point */

		    if ((mag1 > 0.0) || (mag2 > 0.0)) {
		        nms(x, y) = (unsigned char) NOEDGE;
		    }
		    else {
		        if (mag2 == 0.0)
		            nms(x, y) = (unsigned char) NOEDGE;
		        else
		            nms(x, y) = (unsigned char) POSSIBLE_EDGE;
		    }
		}
	}
}

/*******************************************************************************
* PROCEDURE: apply_hysteresis
* PURPOSE: This routine finds edges that are above some high threshhold or
* are connected to a high pixel by a path of pixels greater than a low
* threshold.
*******************************************************************************/
void Canny::apply_hysteresis() {
	int r, numedges, lowcount, highcount, lowthreshold, highthreshold,
		i, hist[32768] = {0};
	short int maximum_mag;
	
	/****************************************************************************
	* Initialize the edge map to possible edges everywhere the non-maximal
	* suppression suggested there could be an edge except for the border. At
	* the border we say there can not be an edge because it makes the
	* follow_edges algorithm more efficient to not worry about tracking an
	* edge off the side of the image.
	****************************************************************************/

	/****************************************************************************
	* Compute the histogram of the magnitude image. Then use the histogram to
	* compute hysteresis thresholds.
	****************************************************************************/

	cimg_forXY(nms, x, y) {
		if (nms(x, y) == POSSIBLE_EDGE) {
			edge(x, y) = POSSIBLE_EDGE;
			hist[magnitude(x, y)]++;
		}
		else edge(x, y) = NOEDGE;
	}

	/****************************************************************************
	* Compute the number of pixels that passed the nonmaximal suppression.
	****************************************************************************/
	for (r = 1, numedges = 0; r < 32768; r++) {
		if (hist[r] != 0) maximum_mag = r;
		numedges += hist[r];
	}

	highcount = (int)(numedges * thigh + 0.5);

	/****************************************************************************
	* Compute the high threshold value as the (100 * thigh) percentage point
	* in the magnitude of the gradient histogram of all the pixels that passes
	* non-maximal suppression. Then calculate the low threshold as a fraction
	* of the computed high threshold value. John Canny said in his paper
	* "A Computational Approach to Edge Detection" that "The ratio of the
	* high to low threshold in the implementation is in the range two or three
	* to one." That means that in terms of this implementation, we should
	* choose tlow ~= 0.5 or 0.33333.
	****************************************************************************/
	r = 1;
	numedges = hist[1];
	while ((r < (maximum_mag - 1)) && (numedges < highcount)) {
		r++;
		numedges += hist[r];
	}
	highthreshold = r;
	lowthreshold = (int)(highthreshold * tlow + 0.5);

	if (VERBOSE) {
		cout << "The input low and high fractions of " << tlow << " and " 
		<< thigh << " computed to" << endl;
		cout << "magnitude of the gradient threshold values of: "
		 << lowthreshold << " " << highthreshold << endl;
	}
	/****************************************************************************
	* This loop looks for pixels above the highthreshold to locate edges and
	* then calls follow_edges to continue the edge.
	****************************************************************************/
	cimg_forXY(edge, x, y) {
		if ((edge(x, y) == POSSIBLE_EDGE) && (magnitude(x, y) >= highthreshold)) {
			edge(x, y) = EDGE;
			follow_edges(x, y, lowthreshold);
		} 
	}

	/****************************************************************************
	* Set all the remaining possible edges to non-edges.
	****************************************************************************/
	cimg_forXY(edge, x, y) {
		if (edge(x, y) != EDGE) {
			edge(x, y) = NOEDGE;
		} 
	}
}

/*******************************************************************************
* PROCEDURE: optimize
* PURPOSE: This routine finds edges that are shorter than 20px and delete them.
*******************************************************************************/
void Canny::optimize() {
	cimg_forXY(edge, x, y) {
		if (edge(x, y) == EDGE) {
			connect(x, y, 1);
		}
	}
	cimg_forXY(edge, x, y) {
		if (edge(x, y) == EDGE) {
			// delete the edge points of short edges.
			edge(x, y) = NOEDGE;
		}
		else if (edge(x, y) == PRESERVED) {
			edge(x, y) = EDGE;
		}
	}
}

/*******************************************************************************
* PROCEDURE: connect
* PURPOSE: This is a recursive routine that finds whether an edge point
* is on a long edge and should be preserved. 
*******************************************************************************/
bool Canny::connect(int x, int y, int length) {
	if (x < 0 || x >= w || y < 0 || y >= h) return false;
	if (edge(x, y) == NOEDGE || edge(x, y) == VISITED) return false;
	if (length >= 20 || edge(x, y) == PRESERVED) {
		edge(x, y) = PRESERVED;
		return true;
	}
	edge(x, y) = VISITED;
	int dirX[8] = {1,1,0,-1,-1,-1,0,1},
		dirY[8] = {0,1,1,1,0,-1,-1,-1};
	bool isPreserved = false;
	for (int i = 0; i < 8; i++) {
		isPreserved = isPreserved || connect(x + dirX[i], y + dirY[i], length + 1);
		if (isPreserved) {
			edge(x, y) = PRESERVED;
			return true;
		}
	}
	edge(x, y) = EDGE;
	return false;
}

void Canny::output() {
	sprintf(outfilename, "%s_s_%3.2f_l_%3.2f_h_%3.2f.jpg", infilename,
			sigma, tlow, thigh);
	if(VERBOSE) cout << "Writing the edge image in the file " 
					 << outfilename << "." << endl;
	edge.save(outfilename);
	// sprintf(outfilename, "%s_s_%3.2f_l_%3.2f_h_%3.2f_t_%d_r_%d.jpg", infilename,
	// 		sigma, tlow, thigh, houghThreshold, range);
	// src.save(outfilename);
}

/*******************************************************************************
* PROCEDURE: make_gaussian_kernel
* PURPOSE: Create a one dimensional gaussian kernel.
* NAME: Mike Heath
* DATE: 2/15/96
*******************************************************************************/
void Canny::make_gaussian_kernel(float **kernel, int *windowsize) {
   int i, center;
   float x, fx, sum=0.0;

   *windowsize = 1 + 2 * ceil(2.5 * sigma);
   center = (*windowsize) / 2;

   if(VERBOSE) printf("      The kernel has %d elements.\n", *windowsize);


   if((*kernel = (float *) calloc((*windowsize), sizeof(float))) == NULL){
      fprintf(stderr, "Error callocing the gaussian kernel array.\n");
      exit(1);
   }


   for(i=0;i<(*windowsize);i++){
      x = (float)(i - center);
      fx = pow(2.71828, -0.5*x*x/(sigma*sigma)) / (sigma * sqrt(6.2831853));
      (*kernel)[i] = fx;
      sum += fx;
   }

   for(i=0;i<(*windowsize);i++) (*kernel)[i] /= sum;

   if(VERBOSE){
      printf("The filter coefficients are:\n");
      for(i=0;i<(*windowsize);i++)
         printf("kernel[%d] = %f\n", i, (*kernel)[i]);
   }
}

/*******************************************************************************
* FUNCTION: angle_radians
* PURPOSE: This procedure computes the angle of a vector with components x and
* y. It returns this angle in radians with the answer being in the range
* 0 <= angle <2*PI.
*******************************************************************************/
double Canny::angle_radians(double x, double y) {
	double xu, yu, ang;

	xu = fabs(x);
	yu = fabs(y);

	if ((xu == 0) && (yu == 0)) return(0);

	ang = atan(yu / xu);

	if (x >= 0) {
		if (y >= 0) return(ang);
		else return(2 * M_PI - ang);
	}
	else {
		if (y >= 0) return(M_PI - ang);
		else return(M_PI + ang);
	}
}

/*******************************************************************************
* PROCEDURE: follow_edges
* PURPOSE: This procedure edges is a recursive routine that traces edgs along
* all paths whose magnitude values remain above some specifyable lower
* threshhold.
*******************************************************************************/
void Canny::follow_edges(int x, int y, int lowthreshold) {
	int tempX, tempY;
	int i;
	float thethresh;
	int dirX[8] = {1,1,0,-1,-1,-1,0,1},
		dirY[8] = {0,1,1,1,0,-1,-1,-1};

	for (i = 0; i < 8; i++) {
		tempX = x + dirX[i];
		tempY = y + dirY[i];

		if((edge(tempX, tempY) == POSSIBLE_EDGE) 
			&& (magnitude(tempX, tempY) > lowthreshold)){
			edge(tempX, tempY) = (unsigned char) EDGE;
			follow_edges(tempX, tempY, lowthreshold);
		}
	}
}