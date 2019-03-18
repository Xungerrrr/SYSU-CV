#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
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
        Point() {
            a = 0;
            b = 0;
            v = 0;
        }
};

class Hough {
    private:
        const char *infilename = NULL;  		/* Name of the input image 			*/
        char outfilename[128];    		        /* Name of the output "edge" image 	*/
        int w, h;				  		        /* The dimensions of the image. 	*/

        CImg<unsigned char> src,
        					edge,               /* The input image 					*/
                            line;               /* The grey scale image 			*/
        CImg<short> hough;
        vector<Point> peaks;
        int houghThreshold;
        int range;
        void output();							/* Write out the edge image to a file. */
        vector<Point> intersections;

        void hough_transform();
        void find_peak_points();
        void draw_lines();
        void draw_intersections();
    public:
        Hough(const char*, CImg<unsigned char> , int, int);	/* The constructor. */
        vector<Point> line_detection();						/* Start the edge detection process. */
};

