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
	CImg<unsigned char> dst;		// ԭͼ��
	CImg<unsigned char> src;		// ��ƴ���ͼ��
	CImg<float> homographyForward;	// ǰ��ӳ�����
	CImg<float> homographyBackward;	// ����ӳ�����
	CImg<unsigned char> stitched;	// ƴ�Ӻ��ͼ��
	vector<VlSiftKeypoint> kpts;	// ��ƴ��ͼ��������㼯��
	vector<float*> descriptors;		// ��ƴ��ͼ��������������Ӽ���
public:
	Stitch(CImg<unsigned char> dst, CImg<unsigned char> src,
		CImg<float> hF, CImg<float> hB, vector<VlSiftKeypoint> kpts, vector<float*> descriptors);
	int calculateWidthAndHeight();	// ��������ͼ�����Ϣ������ƴ��ͼ��Ĵ�С
	Feature stitch();				// ͼ��ƴ��
};