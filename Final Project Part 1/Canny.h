#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include "CImg.h"
using namespace cimg_library;
using namespace std;

#define VERBOSE 1
#define BOOSTBLURFACTOR 90.0
#define NOEDGE 0
#define POSSIBLE_EDGE 128
#define EDGE 255
#define VISITED 1
#define PRESERVED 2
#define THRESHOLD 200

class Canny {
    private:
        int w, h;				  		/* The dimensions of the image. 				*/
        CImg<unsigned char> src,  		/* The input image 								*/
                            image,		/* The grey scale image 						*/
                            edge, 		/* The output edge image 						*/
							nms;        /* Points that are local maximal magnitude. 	*/
        CImg<short int> smoothedim,     /* The image after gaussian smoothing.      	*/
                        delta_x,        /* The first devivative image, x-direction. 	*/
                        delta_y,        /* The first derivative image, y-direction. 	*/
                        magnitude;      /* The magnitude of the gadient image.      	*/
        float sigma,              		/* Standard deviation of the gaussian kernel. 	*/
              tlow,               		/* Fraction of the high threshold in hysteresis.*/
              thigh;              		/* High hysteresis threshold control. The actual
		                                   threshold is the (100 * thigh) percentage 
		                                   point in the histogram of the magnitude of 
		                                   the gradient image that passes non-maximal
		                                   suppression. 								*/
        void to_grey_scale();           			/* Convert the image to grey scale. 				*/
        void gaussian_smooth();						/* Perform gaussian smoothing on the 
        											   image using the input standard deviation.		*/
        void derrivative_x_y();						/* Compute the first derivative in the 
        											   x and y directions.  							*/
        void magnitude_x_y();						/* Compute the magnitude of the gradient. 			*/
        void non_max_supp();						/* Perform non-maximal suppression. 				*/
        void apply_hysteresis();					/* Use hysteresis to mark the edge pixels. 			*/
        void optimize();							/* Delete edges that are shorter than 20 px. 		*/

        void make_gaussian_kernel(float**, int*); 	/* Create a one dimensional gaussian kernel. 		*/
        void follow_edges(int, int, int);	 	  	/* A recursive routine that traces edgs along
													   all paths whose magnitude values remain above 
													   some specifyable lower threshhold. 				*/
        bool connect(int, int, int);				/* A recursive routine that finds whether an edge
													   point should be preserved. 						*/
    public:
        Canny(CImg<unsigned char>, float, float, float);	/* The constructor. */
        CImg<unsigned char> edge_detection();						/* Start the edge detection process. */
};

