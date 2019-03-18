#pragma once
#include "pch.h"
#include <iostream>
#include <vector>
#include "CImg.h"
extern "C" {
#include <vl/generic.h>
#include <vl/sift.h>
}
using namespace cimg_library;
using namespace std;

class Feature {
private:
	CImg<unsigned char> src;		// 原图像
	CImg<float> img;				// 灰度图像
	vector<VlSiftKeypoint> kpts;	// SIFT关键点的集合
	vector<float*> descriptors;		// SIFT关键点描述子的集合

public:
	Feature();
	Feature(CImg<unsigned char> image);

	void toGreyScale();								// 将图像转为灰度图
	void extractFeatures();							// 提取图像的SIFT特征

	// getter and setter
	vector<VlSiftKeypoint> getKeypoints() { return kpts; }
	vector<float*> getDescriptors() { return descriptors; }
	CImg<unsigned char> getImage() { return src; }
	void setKeypoints(vector<VlSiftKeypoint> k) { kpts = k; }
	void setDescriptors(vector<float*> d) {	descriptors = d; }
};