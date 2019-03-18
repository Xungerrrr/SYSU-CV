#pragma once
#include "pch.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <set>
#include "CImg.h"
extern "C" {
#include <vl/generic.h>
#include <vl/sift.h>
#include <vl/kdtree.h>
}
using namespace cimg_library;
using namespace std;

struct pointPair {
	VlSiftKeypoint a;
	VlSiftKeypoint b;
	pointPair(VlSiftKeypoint _a, VlSiftKeypoint _b) {
		a = _a;
		b = _b;
	}
};

class Match {
private:
	// 两幅待匹配图像
	CImg<unsigned char> img1;
	CImg<unsigned char> img2;

	// 图像的特征点集合
	vector<VlSiftKeypoint> kpts1;
	vector<VlSiftKeypoint> kpts2;

	// 图像的特征点描述子集合
	vector<float*> descriptors1;
	vector<float*> descriptors2;

	// 匹配点对和过滤后的匹配点对
	vector<pointPair> matchPoints;
	vector<pointPair> realMatchPoints;

	// 拟合出的单应矩阵
	CImg<double> H;

public:
	Match(CImg<unsigned char> img1, vector<VlSiftKeypoint> kpts1, vector<float*> descriptors1, 
		CImg<unsigned char> img2, vector<VlSiftKeypoint> kpts2, vector<float*> descriptors2);
	void findMatchPoints();	// 特征点匹配
	void showMatchPoints();	// 显示匹配点
	CImg<float> RANSAC();	// RANSAC过程
};