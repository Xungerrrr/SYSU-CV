#include "pch.h"
#include "Feature.h"

Feature::Feature() {}
Feature::Feature(CImg<unsigned char> image) {
	src = image;
	img.assign(src.width(), src.height(), 1, 1, 0);
}

void Feature::toGreyScale()
{
	cimg_forXY(src, x, y) {
		int r = src(x, y, 0);
		int g = src(x, y, 1);
		int b = src(x, y, 2);
		int grey = (r * 30 + g * 59 + b * 11 + 50) / 100;
		img(x, y) = grey;
	}
}

void Feature::extractFeatures() {
	toGreyScale();

	// 初始化VlSiftFilt，总共计算4个塔，每个塔的尺度空间有3层
	VlSiftFilt* vl_sift = vl_sift_new(img.width(), img.height(), 4, 3, 0);
	// 设置剔除不稳定关键点的阈值
	vl_sift_set_peak_thresh(vl_sift, 0.04);
	vl_sift_set_edge_thresh(vl_sift, 10);
	// 将灰度图的数据作为SIFT特征提取所需的数据
	vl_sift_pix *data = (vl_sift_pix*)(img.data());

	// 在每一个塔，计算特征点和特征点描述子
	if (vl_sift_process_first_octave(vl_sift, data) != VL_ERR_EOF) {
		while (true) {
			vl_sift_detect(vl_sift);
			// 得到关键点
			VlSiftKeypoint* pKpts = vl_sift->keys;

			for (int i = 0; i < vl_sift->nkeys; i++) {
				double angles[4];

				// 计算特征点的方向，包括主方向和辅方向，最多4个
				int angleCount = vl_sift_calc_keypoint_orientations(vl_sift, angles, pKpts);

				// 对于方向多于一个的特征点，每个方向分别计算特征描述子
				// 并且将特征点复制多个
				for (int i = 0; i < angleCount; i++) {
					float *des = new float[128];
					vl_sift_calc_keypoint_descriptor(vl_sift, des, pKpts, angles[0]);
					descriptors.push_back(des);
					kpts.push_back(*pKpts);
				}
				pKpts++;
			}
			// 处理下一个塔
			if (vl_sift_process_next_octave(vl_sift) == VL_ERR_EOF) {
				break;
			}
		}
	}
	// 释放资源
	vl_sift_delete(vl_sift);
}