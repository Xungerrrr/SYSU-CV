#include "OSTU.h"

OSTU::OSTU(CImg<unsigned char> _img, int _edge) {
	img = _img;	
	edge = _edge;
	// 将边缘置0
	cimg_forXY(img, x, y) {
		if (x < edge || y < edge ||
			x >= img.width() - edge || y >= img.height() - edge) {
			img(x, y) = 0;
		}
	}	
}
CImg<unsigned char> OSTU::ostu() {
	// 将图像分成200×200的块，逐块进行分割
	for (int x0 = edge; x0 < img.width() - edge; x0 += 200) {
		for (int y0 = edge; y0 < img.height() - edge; y0 += 200) {
			int x1 = x0 + 199 < img.width() - edge ? x0 + 199 : img.width() - edge - 1;
			int y1 = y0 + 199 < img.height() - edge ? y0 + 199 : img.height() - edge - 1;
			CImg<unsigned char> crop = img.get_crop(x0, y0, x1, y1); // 图像块
			for (int i = 0; i < 256; i++) { // 初始化直方图
				hist[i] = 0;
			}
			cimg_forXY(crop, x, y) { // 统计直方图
				hist[(int)crop(x, y)]++;
			}
			double maxVariance = 0;	// 当前最大方差
			int pixelNum = crop.width() * crop.height(); 	// 像素数量
			// 遍历所有灰度值，找到使类间方差最大的灰度值作为分割阈值（OSTU）
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
			int lowNum = 0;
			double w0;
			for (int i = 0; i <= threshold; i++) {
				lowNum += hist[i];
			}
			w0 = (double)lowNum / (double)pixelNum;	// 前景像素占总像素的比例
			if (w0 <= 0.08) { // 前景占比较小，分割
				crop = divide(crop, threshold);
			}
			else {	// 否则不分割，全部置0
				cimg_forXY(crop, x, y) {
					crop(x, y) = 0;
				}
			}
			// 将处理后的块放回原图中
			for (int i = x0; i <= x1; i++) {
				for (int j = y0; j <= y1; j++) {
					img(i, j) = crop(i - x0, j - y0);
				}
			}
		}
	}
	return img;
}
CImg<unsigned char> OSTU::divide(CImg<unsigned char> crop, int threshold) {
	// 二值化
	cimg_forXY(crop, x, y) {
		if (crop(x, y) <= threshold) {
			crop(x, y) = 255;
		}
		else {
			crop(x, y) = 0;
		}
	}
	return crop;
}