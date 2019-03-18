#include <iostream>
#include <cstdio>
#include "Cutting.h"

int main(int argc, char *argv[]) {
	if (argc < 3) {
		fprintf(stderr, "\n<USAGE> %s image outputname\n", argv[0]);
		exit(1);
	}
	char *infilename = NULL, *outputname = NULL;
    infilename = argv[1];
	outputname = argv[2];
	CImg<unsigned char> image(infilename);
	Cutting cutting(image, outputname);
	cutting.divideToLines();
	cutting.divideToSingleNumber();
	cutting.save();
    return 0;
}