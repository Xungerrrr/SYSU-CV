#include <cmath>
#include "CImg.h"
using namespace cimg_library;
using namespace std;

class MyImg {
    private:
        // CImg对象
        CImg<unsigned char> img;

    public:
        // 构造函数
        MyImg(const char* path) {
            img.load_bmp(path);
        }

        // 显示图像
        void display() {
            img.display();
        }

        // 保存图像
        void save(const char* path) {
            img.save(path);
        }

        // 改变颜色
        void change_color() {
            cimg_forXY(img, x, y) {
                if (img(x, y, 0) >= 112 && img(x, y, 1) >= 112 && img(x, y, 2) >= 112) {
                    // 白色区域变红
                    img(x, y, 1) = 0;
                    img(x, y, 2) = 0;
                } else if (img(x, y, 0) == 0 && img(x, y, 1) == 0 && img(x, y, 2) == 0) {
                    // 黑色区域变绿
                    img(x, y, 1) = 255;
                }
            }
        }

        // 不调用CImg函数画蓝色圆
        void draw_blue_circle_1() {
            cimg_forXY(img, x, y) {
                // 当前像素与(50, 50)的距离
                double distance = sqrt(pow(x - 50, 2) + pow(y - 50, 2));

                if (distance <= 30.5) {
                    img(x, y, 0) = 0;
                    img(x, y, 1) = 0;
                    img(x, y, 2) = 255;
                }
            }
        }

        // 调用CImg函数画蓝色圆
        void draw_blue_circle_2() {
            unsigned char blue[] = {0, 0, 255};
            img.draw_circle(50, 50, 30, blue);
        }

        // 不调用CImg函数画黄色圆
        void draw_yellow_circle_1() {
            cimg_forXY(img, x, y) {
                // 当前像素与(50, 50)的距离
                double distance = sqrt(pow(x - 50, 2) + pow(y - 50, 2));

                if (distance <= 3.5) {
                    img(x, y, 0) = 255;
                    img(x, y, 1) = 255;
                    img(x, y, 2) = 0;
                }
            }
        }

        // 调用CImg函数画黄色圆
        void draw_yellow_circle_2() {
            unsigned char yellow[] = {255, 255, 0};
            img.draw_circle(50, 50, 3, yellow);
        }

        // 不调用CImg函数画蓝色线（Bresenham算法）
        void draw_line_1() {
            // 直线终点的x坐标
            double x_end = 100 * cos(35 * M_PI / 180);

            cimg_forXY(img, x, y) {
                if (x <= x_end) {
                    // x坐标对应的理论y坐标
                    double y_1 = tan(35 * M_PI / 180) * x;

                    // 若y_1坐标位于[y - 0.5, y + 0.5)区间内，则填充蓝色
                    if (fabs(y - y_1) < 0.5 || y - y_1 == 0.5) {
                        img(x, y, 0) = 0;
                        img(x, y, 1) = 0;
                        img(x, y, 2) = 255;
                    }
                }
            }
        }

        // 调用CImg函数画蓝色线
        void draw_line_2() {
            unsigned char blue[] = {0, 0, 255};
            img.draw_line(0, 0,
                100 * cos(35 * M_PI / 180), 100 * sin(35 * M_PI / 180), blue);
        }
};