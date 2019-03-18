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

	// ����ԭͼ�񾭹�ǰ��ӳ�������x����ֵ��y����ֵ
	// �õ�ƴ��ͼ��Ŀ�͸�
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
	// ����ƴ�Ӻ�ͼ��Ŀ�͸�
	int yOffset = calculateWidthAndHeight();
	// ��ԭͼ�ƶ���ƴ�Ӻ�λ��
	cimg_forXYC(dst, x, y, c) {
		if (ceil(y+yOffset) < stitched.height())
			stitched(x, y + yOffset, c) = dst(x, y, c);
	}
	// �Դ�ƴ��ͼ������������ǰ��ӳ�䣬�ƶ�������
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
	// ��ƴ�Ӻ�ͼ���з���ӳ�䣬�õ���ƴ��ͼ���Ӧ������ֵ�����ƴ��
	// ����������ȷ��ƴ��ͼ�񲻳��ֿն�
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
	// ��ȡƴ�Ӻ�ͼ����������������������
	Feature f(stitched);
	f.setDescriptors(descriptors);
	f.setKeypoints(kpts);
	return f;
}
