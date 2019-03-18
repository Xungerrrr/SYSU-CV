#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include "CImg.h"
using namespace cimg_library;
using namespace std;

#define VERBOSE 1
#define NOEDGE 0
#define POSSIBLE_EDGE 128
#define EDGE 255

class Point {
    public:
        int a, b, v;
        Point(int _a, int _b, int _v) {
            a = _a;
            b = _b;
            v = _v;
        }
};

class Hough {
    private:
        char *infilename = NULL;  		/* Name of the input image 						*/
        char outfilename[128];    		/* Name of the output "circle" image 				*/
        int w, h;				  		/* The dimensions of the image. 				*/

        CImg<unsigned char> src,  		/* The input image 								*/
                            edge, 		/* The output edge image 						*/
                            circle;     /* The output circle image                        */
        CImg<short> hough;              /* The hough space                              */

        vector<vector<Point> > peakGroups;  /* The peak points groups in the hough space */
        vector<Point> circles;              /* Detected circles */
        int houghThreshold,
            minRadius,
            maxRadius;
        float fraction;
        void output();								/* Write out the edge image to a file. 				*/
        void hough_transform();

    public:
        Hough(char*, CImg<unsigned char>, int, float, int, int);	/* The constructor. */
        void circle_detection();						/* Start the edge detection process. */
};

