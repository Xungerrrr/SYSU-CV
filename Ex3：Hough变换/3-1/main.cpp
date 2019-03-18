#include "Canny.h"
#include "Hough.h"

int main(int argc, char *argv[]) {
    char *infilename = NULL;  /* Name of the input image */
    char *dirfilename = NULL; /* Name of the output gradient direction image */
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
    if (argc < 7){
        fprintf(stderr, "\n<USAGE> %s image sigma tlow thigh houghThreshold range [writedirim]\n", argv[0]);
        fprintf(stderr, "\n      image:      An image to process. Must be in ");
        fprintf(stderr, "PGM format.\n");
        fprintf(stderr, "      sigma:      Standard deviation of the gaussian");
        fprintf(stderr, " blur kernel.\n");
        fprintf(stderr, "      tlow:       Fraction (0.0-1.0) of the high ");
        fprintf(stderr, "edge strength threshold.\n");
        fprintf(stderr, "      thigh:      Fraction (0.0-1.0) of the distribution");
        fprintf(stderr, " of non-zero edge\n                  strengths for ");
        fprintf(stderr, "hysteresis. The fraction is used to compute\n");
        fprintf(stderr, "                  the high edge strength threshold.\n");
        fprintf(stderr, "      writedirim: Optional argument to output ");
        fprintf(stderr, "a floating point");
        fprintf(stderr, " direction image.\n\n");
        exit(1);
    }

    infilename = argv[1];
    sigma = atof(argv[2]);
    tlow = atof(argv[3]);
    thigh = atof(argv[4]);
    houghThreshold = atof(argv[5]);
    range = atof(argv[6]);

    if (argc == 8) dirfilename = infilename;
    else dirfilename = NULL;

    /****************************************************************************
    * Create a Canny object and start the edge detection process.
    ****************************************************************************/
    Canny cny(infilename, sigma, tlow, thigh, dirfilename);
    CImg<unsigned char> edge = cny.edge_detection();
    
    Hough hough(infilename, edge, houghThreshold, range);
    hough.line_detection();
    return 0;
}
