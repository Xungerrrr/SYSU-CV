#include <cmath>
#include <iostream>
#include "CImg.h"
using namespace cimg_library;
using namespace std;

class HistEq {
    private:
        CImg<double> img;   // 待处理的图像
        int hist[256];      // 图像的直方图
        double map[256];    // 原图的像素值到均衡化图像的像素值的映射

    public:
        // 构造函数
        HistEq(const char* path) {
            img.load(path);
        	for (int i = 0; i < 256; i++) {
        		hist[i] = 0;
        		map[i] = 0;
        	}	
        }

        // 显示图像
        void display() {
            img.display();
        }

        // 保存图像
        void save(const char* path) {
            img.save(path);
        }

        // 计算图像直方图
        void calculateHist() {
            // 如果是彩色图，则将图像转到HSI颜色空间，
            // 并计算I通道（亮度通道）的直方图。
            if (img.spectrum() == 3) {
                img.RGBtoHSI();
                // 划分100个亮度等级
                img *= 100;
                cimg_forXY(img, x, y) {
                    hist[(int)img(x, y, 2)]++;
                }
            }
            // 如果是灰度图，则直接计算图像直方图。
            else if (img.spectrum() == 1) {
                cimg_forXY(img, x, y) {
                    hist[(int)img(x, y)]++;
                }
            }
        	
        }

        // 使用累积分布函数（cdf），进行直方图均衡化，
        // 得到原图像亮度与均衡后图像亮度之间的映射关系
        void calculateMap() {
        	for (int i = 0; i < 256; i++) {
                // map(r)=L/MN ∑_(i=0)^r hist(i)
                // L为均衡化等级数量，MN为像素总数
        		int sum = 0;
        		for (int j = 0; j <= i; j++) {
        			sum += hist[j];
        		}
                if (img.spectrum() == 3) {
                    map[i] = 100 * sum / (img.width() * img.height());
                }
                else if (img.spectrum() == 1) {
                    map[i] = 255 * sum / (img.width() * img.height());
                }
        	}
        }

        // 利用映射关系，修改原图像
        void apply() {
        	cimg_forXY(img, x, y) {
                // 若为彩色图像，则修改I通道（亮度通道)
                if (img.spectrum() == 3) {
                    img(x, y, 2) = map[(int)img(x, y, 2)];
                }
                else if (img.spectrum() == 1) {
                    img(x, y) = map[(int)img(x, y)];
                }
        	}
            // 若为彩色图，将图像转换回RGB空间
            if (img.spectrum() == 3) {
                img /= 100;
                img.HSItoRGB();
            }
        }
};