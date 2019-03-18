#include <iostream>
#include <vector>
#include <cstdio>
#include <fstream>
#include <list>
#include "CImg.h"
#include "Hough.h"

using namespace cimg_library;
using namespace std;

class Cutting {
private:
    const char* outputname;     // 输出文件夹的名称
    CImg<float> histY, histX;   // Y方向和X方向的直方图
    CImg<unsigned char> binary; // 输入的二值化图像
    vector<int> cutY;           // Y方向的切割坐标集合
    vector<CImg<unsigned char> > rows;  // 切割出的行图像集合
    vector<vector<CImg<unsigned char> > > numbers;  // 切割出的单个数字集合
public:
    Cutting(CImg<unsigned char> img, const char* filename) {
        binary = img;
        outputname = filename;
    }
    void divideToLines();           // 切割行
    void divideToSingleNumber();    // 切割列
    void save();                    // 保存所有数字图像
};