#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "CImg.h"
using namespace cimg_library;
using namespace std;

#define VERBOSE 1
#define BOOSTBLURFACTOR 90.0
#define NOEDGE 255
#define POSSIBLE_EDGE 128
#define EDGE 0
#define VISITED 1
#define PRESERVED 2

class Canny {
    private:
        char *infilename = NULL;  		/* Name of the input image 						*/
        char *dirfilename = NULL; 		/* Name of the output gradient direction image  */
        FILE *fpdir = NULL;          	/* File to write the gradient image to. 		*/
        char outfilename[128];    		/* Name of the output "edge" image 				*/
        char composedfname[128];  		/* Name of the output "direction" image 		*/
        int w, h;				  		/* The dimensions of the image. 				*/
        float *dir_radians = NULL;   	/* Gradient direction image.                	*/
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
        void radian_direction();					/* Compute the direction of the gradient.(optional) */
        void magnitude_x_y();						/* Compute the magnitude of the gradient. 			*/
        void non_max_supp();						/* Perform non-maximal suppression. 				*/
        void apply_hysteresis();					/* Use hysteresis to mark the edge pixels. 			*/
        void optimize();							/* Delete edges that are shorter than 20 px. 		*/
        void output();								/* Write out the edge image to a file. 				*/

        void make_gaussian_kernel(float**, int*); 	/* Create a one dimensional gaussian kernel. 		*/
        double angle_radians(double, double);	  	/* Computes the angle of a vector 					*/
        void follow_edges(int, int, int);	 	  	/* A recursive routine that traces edgs along
													   all paths whose magnitude values remain above 
													   some specifyable lower threshhold. 				*/
        bool connect(int, int, int);				/* A recursive routine that finds whether an edge
													   point should be preserved. 						*/
    public:
        Canny(char*, float, float, float, char*);	/* The constructor. */
        void edge_detection();						/* Start the edge detection process. */
};