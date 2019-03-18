#include "pch.h"
#include "Stitch.h"

Stitch::Stitch(CImg<unsigned char> d, CImg<unsigned char> s, CImg<float> hF, CImg<float> hB, vector<VlSiftKeypoint> k, vector<float*> descript)
{
	dst = d;
	src = s;
	homographyForward = hF;
	homographyBackward = hB;
	kpts = k;
	descriptors = descript;
}

int Stitch::calculateWidthAndHeight()
{
	int width, height, minY, maxY;
	height = src.height();
	int yOffset = 0;

	// 计算原图像经过前向映射后，最大的x坐标值和y坐标值
	// 得到拼接图像的宽和高
	CImg<float> xy1(1, 3, 1, 1, 1);
	xy1(0) = src.width() - 1;
	xy1(1) = 0;
	CImg<float> target = homographyForward * xy1;
	target /= target(2);
	width = ceil(target(0));
	minY = floor(target(1));

	xy1(1) = src.height() - 1;
	target = homographyForward * xy1;
	target /= target(2);
	if (ceil(target(0)) > width)
		width = ceil(target(0));
	maxY = ceil(target(1));

	xy1(0) = 0;
	xy1(1) = 0;
	target = homographyForward * xy1;
	target /= target(2);
	if (floor(target(1)) < minY)
		minY = floor(target(1));

	xy1(0) = 0;
	xy1(1) = src.height() - 1;
	target = homographyForward * xy1;
	target /= target(2);
	if (ceil(target(1)) > maxY)
		maxY = ceil(target(1));

	if (maxY + 1 > src.height())
		height = maxY + 1;
	if (minY < 0) {
		yOffset = abs(minY);
		height += yOffset;
	}
	stitched.assign(width, height, 1, 3, 0);
	return yOffset;
}

Feature Stitch::stitch()
{
	// 计算拼接后图像的宽和高
	int yOffset = calculateWidthAndHeight();
	// 将原图移动到拼接后位置
	cimg_forXYC(dst, x, y, c) {
		if (ceil(y+yOffset) < stitched.height())
			stitched(x, y + yOffset, c) = dst(x, y, c);
	}
	// 对待拼接图像的特征点进行前向映射，移动特征点
	for (int i = 0; i < kpts.size(); i++) {
		CImg<float> xy1(1, 3, 1, 1, 1);
		xy1(0) = kpts[i].x;
		xy1(1) = kpts[i].y;
		CImg<float> target = homographyForward * xy1;
		target /= target(2);
		kpts[i].x = target(0);
		kpts[i].y = target(1) + yOffset;
		kpts[i].ix = (int)kpts[i].x;
		kpts[i].iy = (int)kpts[i].y;
	}
	// 从拼接后图像中反向映射，得到待拼接图像对应的像素值，完成拼接
	// 这样做可以确保拼接图像不出现空洞
	cimg_forXYC(stitched, x, y, c) {
		CImg<float> xy1(1, 3, 1, 1, 1);
		xy1(0) = x;
		xy1(1) = y - yOffset;
		CImg<float> target = homographyForward * xy1;
		target = homographyBackward * xy1;
		target /= target(2);
		float target_x = target(0);
		float target_y = target(1);
		if (target_x >= 0 && target_x < src.width() && target_y >= 0 && target_y < src.height()) {
			stitched(x, y, c) = src((int)target_x, (int)target_y, c);
		}
	}
	stitched.display();
	// 获取拼接后图像的特征点和特征点描述子
	Feature f(stitched);
	f.setDescriptors(descriptors);
	f.setKeypoints(kpts);
	return f;
}
