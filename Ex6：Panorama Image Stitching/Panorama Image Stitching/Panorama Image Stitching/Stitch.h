#pragma once
#include "pch.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <set>
#include "CImg.h"
#include "Feature.h"
extern "C" {
#include <vl/generic.h>
#include <vl/sift.h>
#include <vl/kdtree.h>
}
using namespace cimg_library;
using namespace std;

class Stitch {
private:
	CImg<unsigned char> dst;		// 原图像
	CImg<unsigned char> src;		// 待拼入的图像
	CImg<float> homographyForward;	// 前向映射矩阵
	CImg<float> homographyBackward;	// 后向映射矩阵
	CImg<unsigned char> stitched;	// 拼接后的图像
	vector<VlSiftKeypoint> kpts;	// 待拼入图像的特征点集合
	vector<float*> descriptors;		// 待拼入图像的特征点描述子集合
public:
	Stitch(CImg<unsigned char> dst, CImg<unsigned char> src,
		CImg<float> hF, CImg<float> hB, vector<VlSiftKeypoint> kpts, vector<float*> descriptors);
	int calculateWidthAndHeight();	// 根据两幅图像的信息，计算拼接图像的大小
	Feature stitch();				// 图像拼接
};