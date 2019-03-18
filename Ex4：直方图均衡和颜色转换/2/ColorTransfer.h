#include <cmath>
#include "CImg.h"
#include <iostream>
using namespace cimg_library;
using namespace std;

class ColorTransfer {
    private:
        CImg<double> source;    // 原图像
        CImg<double> target;    // 目标图像
        double sourceMean[3];   // 原图像三个通道的均值
        double targetMean[3];   // 目标图像三个通道的均值
        double sourceSD[3];     // 原图像三个通道的标准差
        double targetSD[3];     // 目标图像三个通道的标准差

    public:
        // 构造函数
        ColorTransfer(const char* path1, const char* path2) {
            source.load(path1);
            target.load(path2);
        }

        // 显示图像
        void display() {
            source.display();
            target.display();
        }

        // 保存图像
        void save(const char* path) {
            source.save(path);
        }

        // 将图像转到Lab颜色空间
        void RGBtoLab() {
        	source.RGBtoLab();
        	target.RGBtoLab();
        }

        // 将图像转回RGB颜色空间
        void LabtoRGB() {
            source.LabtoRGB();
            target.LabtoRGB();
        }

        // 计算两幅图像各个通道的均值
        void calculateMean() {
            // 计算原图均值
            double lSum = 0, aSum = 0, bSum = 0;
        	cimg_forXY(source, x, y) {
	        	lSum += source(x, y, 0);
                aSum += source(x, y, 1);
                bSum += source(x, y, 2);
	        }
            sourceMean[0] = lSum / (source._width * source._height);
            sourceMean[1] = aSum / (source._width * source._height);
            sourceMean[2] = bSum / (source._width * source._height);

            // 计算目标图像均值
            lSum = 0, aSum = 0, bSum = 0;
            cimg_forXY(target, x, y) {
                lSum += target(x, y, 0);
                aSum += target(x, y, 1);
                bSum += target(x, y, 2);
            }
            targetMean[0] = lSum / (target._width * target._height);
            targetMean[1] = aSum / (target._width * target._height);
            targetMean[2] = bSum / (target._width * target._height);
        }

        // 计算两幅图像各个通道的标准差
        void calculateSD() {
            // 计算原图标准差
            double lSD = 0, aSD = 0, bSD = 0;
            cimg_forXY(source, x, y) {
                lSD += pow(source(x, y, 0) - sourceMean[0], 2);
                aSD += pow(source(x, y, 1) - sourceMean[1], 2);
                bSD += pow(source(x, y, 2) - sourceMean[2], 2);
            }
            sourceSD[0] = sqrt(lSD / (source._width * source._height));
            sourceSD[1] = sqrt(aSD / (source._width * source._height));
            sourceSD[2] = sqrt(bSD / (source._width * source._height));

            // 计算目标图像标准差
            lSD = 0, aSD = 0, bSD = 0;
            cimg_forXY(target, x, y) {
                lSD += pow(target(x, y, 0) - targetMean[0], 2);
                aSD += pow(target(x, y, 1) - targetMean[1], 2);
                bSD += pow(target(x, y, 2) - targetMean[2], 2);
            }
            targetSD[0] = sqrt(lSD / (target._width * target._height));
            targetSD[1] = sqrt(aSD / (target._width * target._height));
            targetSD[2] = sqrt(bSD / (target._width * target._height));
        }

        // 颜色矫正（转换）
        void colorCorrection() {
        	cimg_forXY(source, x, y) {
                double l, a, b;
                // 求出三个通道转换后的值
                l = (targetSD[0] / sourceSD[0]) * (source(x, y, 0) 
                    - sourceMean[0]) + targetMean[0];
                a = (targetSD[1] / sourceSD[1]) * (source(x, y, 1) 
                    - sourceMean[1]) + targetMean[1];
                b = (targetSD[2] / sourceSD[2]) * (source(x, y, 2) 
                    - sourceMean[2]) + targetMean[2];
                source(x, y, 0) = l;
                source(x, y, 1) = a;
                source(x, y, 2) = b;

            }
        }

        
};