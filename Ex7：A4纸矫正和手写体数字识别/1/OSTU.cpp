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
public:
	OSTU(CImg<unsigned char> _img) {
		img = _img;
		// 初始化直方图
		for (int i = 0; i < 256; i++) {
    		hist[i] = 0;
    	}
    	// 统计直方图
    	cimg_forXY(img, x, y) {
            hist[(int)img(x, y)]++;
        }
        
	}
	void ostu() {
		double maxVariance = 0;	// 当前最大方差
		int pixelNum = img.width() * img.height(); 	// 像素数量
		// 遍历所有灰度值，找到使类间方差最大的灰度值作为分割阈值
		for (int i = 0; i < 256; i++) {
			int lowNum = 0, lowValue = 0, highNum = 0, highValue = 0;
			double w0, w1, u0, u1, g;
			for (int j = 0; j <= i; j++) {
				lowNum += hist[j];
				lowValue += j * hist[j];
			}
			for (int j = i + 1; j < 256; j++) {
				highNum += hist[j];
				highValue += j * hist[j];
			}
			w0 = (double)lowNum / (double)pixelNum;		// 前景像素占总像素的比例
			w1 = (double)highNum / (double)pixelNum;	// 背景像素占总像素的比例
			u0 = (double)lowValue / (double)lowNum;		// 前景平均灰度
			u1 = (double)highValue / (double)highNum;	// 背景平均灰度
			g = w0 * w1 * pow((u0 - u1), 2);			// 类间方差
			if (g > maxVariance) {
				maxVariance = g;
				threshold = i;
			}
		}
		cout << "Divide threshold: " << threshold << endl;
	}
	CImg<unsigned char> divide() {
		// 二值化
		cimg_forXY(img, x, y) {
			if (img(x, y) <= threshold) {
				img(x, y) = 0;
			}
			else {
				img(x, y) = 255;
			}
		}
		CImg<unsigned char> edge = img;
		// 8邻域滤波，提取边缘
		cimg_forXY(img, x, y) {
			if (img(x, y) == 255) {
				if (x > 0 && x < img.width() - 1 && y > 0 && y < img.height() - 1) {
					if (img(x - 1, y - 1) == 255 &&
						img(x - 1, y) == 255 &&
						img(x - 1, y + 1) == 255 &&
						img(x, y - 1) == 255 &&
						img(x, y + 1) == 255 &&
						img(x + 1, y - 1) == 255 &&
						img(x + 1, y) == 255 &&
						img(x + 1, y + 1) == 255) {
						edge(x, y) = 0;
					}
				}
				else {
					edge(x, y) = 0;
				}
			}
		}
		return edge;
	}
};