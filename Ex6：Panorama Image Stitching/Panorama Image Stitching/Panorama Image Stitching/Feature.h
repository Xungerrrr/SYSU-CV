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
	CImg<unsigned char> src;		// ԭͼ��
	CImg<float> img;				// �Ҷ�ͼ��
	vector<VlSiftKeypoint> kpts;	// SIFT�ؼ���ļ���
	vector<float*> descriptors;		// SIFT�ؼ��������ӵļ���

public:
	Feature();
	Feature(CImg<unsigned char> image);

	void toGreyScale();								// ��ͼ��תΪ�Ҷ�ͼ
	void extractFeatures();							// ��ȡͼ���SIFT����

	// getter and setter
	vector<VlSiftKeypoint> getKeypoints() { return kpts; }
	vector<float*> getDescriptors() { return descriptors; }
	CImg<unsigned char> getImage() { return src; }
	void setKeypoints(vector<VlSiftKeypoint> k) { kpts = k; }
	void setDescriptors(vector<float*> d) {	descriptors = d; }
};