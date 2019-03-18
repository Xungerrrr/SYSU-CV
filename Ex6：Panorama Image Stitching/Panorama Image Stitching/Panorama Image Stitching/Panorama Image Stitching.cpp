// Panorama Image Stitching.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <vector>
#include "CImg.h"
#include "Feature.h"
#include "Match.h"
#include "Stitch.h"

extern "C" {
#include <vl/generic.h>
#include <vl/sift.h>
}
using namespace cimg_library;
using namespace std;

int main()
{
	// 读取图像
	vector<Feature> src_imgs(18);
	src_imgs[0] = Feature(CImg<unsigned char>("test/1.jpg"));
	src_imgs[1] = Feature(CImg<unsigned char>("test/2.jpg"));
	src_imgs[2] = Feature(CImg<unsigned char>("test/3.jpg"));
	src_imgs[3] = Feature(CImg<unsigned char>("test/4.jpg"));
	src_imgs[4] = Feature(CImg<unsigned char>("test/5.jpg"));
	src_imgs[5] = Feature(CImg<unsigned char>("test/6.jpg"));
	src_imgs[6] = Feature(CImg<unsigned char>("test/7.jpg"));
	src_imgs[7] = Feature(CImg<unsigned char>("test/8.jpg"));
	src_imgs[8] = Feature(CImg<unsigned char>("test/9.jpg"));
	src_imgs[9] = Feature(CImg<unsigned char>("test/10.jpg"));
	src_imgs[10] = Feature(CImg<unsigned char>("test/11.jpg"));
	src_imgs[11] = Feature(CImg<unsigned char>("test/12.jpg"));
	src_imgs[12] = Feature(CImg<unsigned char>("test/13.jpg"));
	src_imgs[13] = Feature(CImg<unsigned char>("test/14.jpg"));
	src_imgs[14] = Feature(CImg<unsigned char>("test/15.jpg"));
	src_imgs[15] = Feature(CImg<unsigned char>("test/16.jpg"));
	src_imgs[16] = Feature(CImg<unsigned char>("test/17.jpg"));
	src_imgs[17] = Feature(CImg<unsigned char>("test/18.jpg"));

	// 提取特征点
	for (int i = 0; i < src_imgs.size(); i++) {
		src_imgs[i].extractFeatures();
	}
	Feature stitched = src_imgs[0];
	
	// 逐张图像拼接
	for (int i = 0; i < src_imgs.size() - 1; i++) {
		Match matchForward(src_imgs[i + 1].getImage(), src_imgs[i + 1].getKeypoints(), src_imgs[i + 1].getDescriptors(),
			stitched.getImage(), stitched.getKeypoints(), stitched.getDescriptors());
		Match matchBackward(stitched.getImage(), stitched.getKeypoints(), stitched.getDescriptors(),
			src_imgs[i + 1].getImage(), src_imgs[i + 1].getKeypoints(), src_imgs[i + 1].getDescriptors());
		// 特征点匹配
		matchForward.findMatchPoints();
		matchBackward.findMatchPoints();
		// 进行RANSAC过程，得到前向和后向映射的变换矩阵
		CImg<float> homographyForward = matchForward.RANSAC();
		CImg<float> homographyBackward = matchBackward.RANSAC();
		
		Stitch stitch(stitched.getImage(), src_imgs[i + 1].getImage(), homographyForward, homographyBackward,
			src_imgs[i + 1].getKeypoints(), src_imgs[i + 1].getDescriptors());
		// 释放前一个拼接图像的特征点描述子占用的资源
		vector<float*> descriptors = stitched.getDescriptors();
		for (int i = 0; i < descriptors.size(); i++) {
			delete[] descriptors[i];
			descriptors[i] = NULL;
		}
		// 图像拼接
		stitched = stitch.stitch();
	}
	// 输出结果
	stitched.getImage().save("result.bmp");
	return 0;
}	
