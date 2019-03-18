#include "Warping.h"
#include "Canny.h"
#include <iostream>

int main(int argc, char *argv[]) {
    char *infilename = NULL,  /* Name of the input image */
         *outputname = NULL;
    float sigma,              /* Standard deviation of the gaussian kernel. */
          tlow,               /* Fraction of the high threshold in hysteresis. */
          thigh;              /* High hysteresis threshold control. The actual
                                 threshold is the (100 * thigh) percentage point
                                 in the histogram of the magnitude of the
                                 gradient image that passes non-maximal
                                 suppression. */
    int houghThreshold,
        range;

    /****************************************************************************
    * Get the command line arguments.
    ****************************************************************************/
    if (argc < 8){
        exit(1);
    }

    infilename = argv[1];
    sigma = atof(argv[2]);
    tlow = atof(argv[3]);
    thigh = atof(argv[4]);
    houghThreshold = atof(argv[5]);
    range = atof(argv[6]);
    outputname = argv[7];

    ofstream out;
    out.open("corner.txt", std::ios::app);
    if (out.is_open()) {
        out << outputname;
        out.close();
    }

    /****************************************************************************
    * Create a Canny object and start the edge detection process.
    ****************************************************************************/

	CImg<unsigned char> src(infilename);
	Canny cny(src, sigma, tlow, thigh);
	CImg<unsigned char> edge = cny.edge_detection();

	Hough hough(edge, houghThreshold, range);
	vector<Point> intersections = hough.line_detection();

	// A4纸矫正
	Warping warping(intersections, src);
	warping.getPointPair();
	warping.calculateHomography();
    CImg<unsigned char> warpingResult = warping.warp();

	// 输出结果
	char result[20];
	sprintf(result, "Step1\\%s", outputname);
	warpingResult.save(result);
	return 0;
}