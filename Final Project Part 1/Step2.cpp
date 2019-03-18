#include <iostream>
#include <cstdio>
#include "OSTU.h"

int main(int argc, char *argv[]) {
	if (argc < 4) {
		fprintf(stderr, "\n<USAGE> %s image edge outputname\n", argv[0]);
		exit(1);
	}
	char *infilename = NULL,
		 *outputname = NULL;
	int edge = 0;
    infilename = argv[1];
	edge = atof(argv[2]);
	outputname = argv[3];
	CImg<unsigned char> src(infilename);
	CImg<unsigned char> image(src.width(), src.height(), 1, 1, 0);

	// 转灰度图
	cimg_forXY(src, x, y) {
		int r = src(x, y, 0);
		int g = src(x, y, 1);
		int b = src(x, y, 2);
		int grey = (r * 30 + g * 59 + b * 11 + 50) / 100;
		image(x, y) = grey;
	}

	// 二值化
	OSTU ostu = OSTU(image, edge);
	CImg<unsigned char> binaryResult = ostu.ostu();

	// 输出结果
	char result[20];
	sprintf(result, "Step2\\%s.jpg", outputname);
	binaryResult.save(result);
    return 0;
}