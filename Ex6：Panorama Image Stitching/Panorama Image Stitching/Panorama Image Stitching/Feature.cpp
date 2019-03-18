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

	// ��ʼ��VlSiftFilt���ܹ�����4������ÿ�����ĳ߶ȿռ���3��
	VlSiftFilt* vl_sift = vl_sift_new(img.width(), img.height(), 4, 3, 0);
	// �����޳����ȶ��ؼ������ֵ
	vl_sift_set_peak_thresh(vl_sift, 0.04);
	vl_sift_set_edge_thresh(vl_sift, 10);
	// ���Ҷ�ͼ��������ΪSIFT������ȡ���������
	vl_sift_pix *data = (vl_sift_pix*)(img.data());

	// ��ÿһ�����������������������������
	if (vl_sift_process_first_octave(vl_sift, data) != VL_ERR_EOF) {
		while (true) {
			vl_sift_detect(vl_sift);
			// �õ��ؼ���
			VlSiftKeypoint* pKpts = vl_sift->keys;

			for (int i = 0; i < vl_sift->nkeys; i++) {
				double angles[4];

				// ����������ķ��򣬰���������͸��������4��
				int angleCount = vl_sift_calc_keypoint_orientations(vl_sift, angles, pKpts);

				// ���ڷ������һ���������㣬ÿ������ֱ��������������
				// ���ҽ������㸴�ƶ��
				for (int i = 0; i < angleCount; i++) {
					float *des = new float[128];
					vl_sift_calc_keypoint_descriptor(vl_sift, des, pKpts, angles[0]);
					descriptors.push_back(des);
					kpts.push_back(*pKpts);
				}
				pKpts++;
			}
			// ������һ����
			if (vl_sift_process_next_octave(vl_sift) == VL_ERR_EOF) {
				break;
			}
		}
	}
	// �ͷ���Դ
	vl_sift_delete(vl_sift);
}