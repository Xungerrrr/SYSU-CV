#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "CImg.h"
using namespace cimg_library;
using namespace std;

#define TOTAL_FRAME 13

class Point {
public:
	int x, y;	// 点的x和y坐标值

	Point(): x(0), y(0) {}
	Point(int _x, int _y): x(_x), y(_y) {}
};

class Triangle {
public:
	Point p1, p2, p3;	// 三角形的三个顶点
	int index[3];		// 三个顶点在原图点集中的下标

	Triangle(Point _p1, Point _p2, Point _p3, int index1, int index2, int index3) {
		p1 = _p1;
		p2 = _p2;
		p3 = _p3;
		index[0] = index1;
		index[1] = index2;
		index[2] = index3;
	}
};

class Morphing {
private:
    CImg<unsigned char> source;				// 原图像
    CImg<unsigned char> target;				// 目标图像
    std::vector<Point> sourcePoint;			// 原图像的标记点
    std::vector<Point> targetPoint;			// 目标图像的标记点
    std::vector<Triangle> sourceTriangle;	// 原图像中的三角划分
    std::vector<Triangle> targetTriangle;	// 目标图像的三角划分
    CImgList<unsigned char> morphing;		// 过渡动画

public:
    Morphing(const char* sourcePath, const char* targetPath);
    void getPoints();						// 获取图像标记点
    void getTriangles();					// 根据标记点，获取图像的三角划分
    void calculateMiddleFrames();			// 计算所有过渡帧
    void save();							// 保存过渡动画
    
    void selectPoints(CImg<unsigned char>& src, 
    	std::vector<Point>& points, const char* filename);				// 从文件中读取或手动标注标记点
    bool isPointInCircle(Point& P, Triangle& tri);						// 判断一个点是否在三角形的外接圆内
    bool isPointInTriangle(Point& P, Triangle& tri);					// 判断一个点是否在三角形内
    CImg<double> calculateTransformMatrix(Triangle from, Triangle to);	// 给定变换前后的三角形，计算仿射变换的矩阵
};