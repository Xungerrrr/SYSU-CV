#include "CImg.h"
#include "Hough.h"
#include <iostream>
#include <fstream>
using namespace cimg_library;
using namespace std;

struct pointPair {
	Point a;
	Point b;
	pointPair(Point _a, Point _b) {
		a = _a;
		b = _b;
	}
};

class Warping {
private:
	vector<pointPair> pointPairs;	// 原图像和目标图像互相对应的点
	vector<Point> intersections;	// A4纸角点	
	CImg<unsigned char> src;		// 原图像
	CImg<unsigned char> dst;		// 目标图像
	CImg<float> H;					// 变换所用到的单应矩阵
public:
	Warping(vector<Point> _point, CImg<unsigned char> _src);
	void getPointPair();
	void calculateHomography();
	CImg<unsigned char> warp();
};