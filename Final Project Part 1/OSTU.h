#include <cmath>
#include <iostream>
#include "CImg.h"
using namespace cimg_library;
using namespace std;

class OSTU {
private:
	CImg<unsigned char> img;	// 待分割的图像
	int hist[256];				// 图像的直方图
	int threshold;				// 分割的阈值
	int edge;
public:
	OSTU(CImg<unsigned char> _img, int _edge);
	CImg<unsigned char> ostu();
	CImg<unsigned char> divide(CImg<unsigned char> crop, int threshold);
};