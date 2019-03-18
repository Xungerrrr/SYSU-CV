#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include "CImg.h"

using namespace cimg_library;
using namespace std;

#define VERBOSE 1
#define NOEDGE 0
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
        char outfilename[128];    		/* Name of the output "line" image 				*/
        int w, h;				  		/* The dimensions of the image. 				*/

        CImg<unsigned char> src,        /* The input image                               */
        					edge,  		/* The edge image 								*/
                            line;		/* The line image 						        */
        CImg<short> hough;              /* The hough space                               */
        vector<Point> peaks;            /* Peak points in the hough space                */
        int houghThreshold;
        int range;
        void output();								/* Write out the line image to a file. 				*/

        void hough_transform();
        void find_peak_points();
        void draw_lines();
        void draw_intersections();
    public:
        Hough(char*, CImg<unsigned char> , int, int);	/* The constructor. */
        void line_detection();						/* Start the line detection process. */
};

