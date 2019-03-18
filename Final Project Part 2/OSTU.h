#include <cmath>
#include <iostream>
#include "CImg.h"
using namespace cimg_library;
using namespace std;

class OSTU {
private:
	CImg<unsigned char> img;	// 待分割的图像
	int hist[256];				// 直方图
	int threshold;				// 分割的阈值
	int edge;					// 图像边缘宽度
public:
	OSTU(CImg<unsigned char> _img, int _edge);
	CImg<unsigned char> ostu(); // 确定每一块的阈值和是否进行分割
	CImg<unsigned char> divide(CImg<unsigned char> crop, int threshold); // 分割
};