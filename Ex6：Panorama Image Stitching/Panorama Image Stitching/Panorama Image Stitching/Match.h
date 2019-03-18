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
	// ������ƥ��ͼ��
	CImg<unsigned char> img1;
	CImg<unsigned char> img2;

	// ͼ��������㼯��
	vector<VlSiftKeypoint> kpts1;
	vector<VlSiftKeypoint> kpts2;

	// ͼ��������������Ӽ���
	vector<float*> descriptors1;
	vector<float*> descriptors2;

	// ƥ���Ժ͹��˺��ƥ����
	vector<pointPair> matchPoints;
	vector<pointPair> realMatchPoints;

	// ��ϳ��ĵ�Ӧ����
	CImg<double> H;

public:
	Match(CImg<unsigned char> img1, vector<VlSiftKeypoint> kpts1, vector<float*> descriptors1, 
		CImg<unsigned char> img2, vector<VlSiftKeypoint> kpts2, vector<float*> descriptors2);
	void findMatchPoints();	// ������ƥ��
	void showMatchPoints();	// ��ʾƥ���
	CImg<float> RANSAC();	// RANSAC����
};