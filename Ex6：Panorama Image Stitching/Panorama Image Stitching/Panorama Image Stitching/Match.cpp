#include "pch.h"
#include "Match.h"
#include <cmath>

Match::Match(CImg<unsigned char> image1, vector<VlSiftKeypoint> k1, vector<float*> d1, CImg<unsigned char> image2, vector<VlSiftKeypoint> k2, vector<float*> d2)
{
	img1 = image1;
	img2 = image2;
	kpts1 = k1;
	kpts2 = k2;
	descriptors1 = d1;
	descriptors2 = d2;
}

void Match::findMatchPoints()
{
	// 初始化kd森林
	VlKDForest* forest = vl_kdforest_new(VL_TYPE_FLOAT, 128, 1, VlDistanceL1);
	// 获取img1全部特征点描述子的数据
	float *data = new float[128 * descriptors1.size()];
	for (int i = 0; i < descriptors1.size(); i++) {
		for (int j = 0; j < 128; j++) {
			data[i * 128 + j] = descriptors1[i][j];
		}
	}
	// 建立kd森林
	vl_kdforest_build(forest, descriptors1.size(), data);

	VlKDForestSearcher* searcher = vl_kdforest_new_searcher(forest);
	VlKDForestNeighbor neighbours[2];

	// 遍历img2的特征点描述子集合，寻找img1的最近邻特征点
	for (int i = 0; i < descriptors2.size(); i++) {
		float *tempData = new float[128];
		for (int j = 0; j < 128; j++) {
			tempData[j] = descriptors2[i][j];
		}
		vl_kdforestsearcher_query(searcher, neighbours, 2, tempData);
		float ratio = neighbours[0].distance / neighbours[1].distance;
		if (ratio < 0.5) {
			VlSiftKeypoint left = kpts1[neighbours[0].index];
			VlSiftKeypoint right = kpts2[i];
			matchPoints.push_back(pointPair(left, right));
		}
		delete[] tempData;
		tempData = NULL;
	}
	// 释放资源
	vl_kdforestsearcher_delete(searcher);
	vl_kdforest_delete(forest);
	delete[] data;
	data = NULL;
}

void Match::showMatchPoints()
{
	float blue[] = { 0, 0, 255 };
	int width = img1.width() + img2.width();
	int height = img1.height();
	CImg<unsigned char> match(width, height, 1, 3, 0);
	cimg_forXYC(img2, x, y, c) {
		match(x, y, c) = img2(x, y, c);
		match(x + img2.width(), y, c) = img1(x, y, c);
	}
	for (int i = 0; i < realMatchPoints.size(); i++) {
		VlSiftKeypoint a = realMatchPoints[i].a;
		VlSiftKeypoint b = realMatchPoints[i].b;
		match.draw_line(b.x, b.y, a.x + img2.width(), a.y, blue);
	}
	match.display();
}

int numberOfIterations(float p, float w, int num) {
	return ceil(log(1 - p) / log(1 - pow(w, num)));
}
int random(int min, int max) {
	assert(max > min);
	return rand() % (max - min + 1) + min;
}

CImg<float> calculateHomography(vector<pointPair> randomPairs) {
	int size = randomPairs.size();
	CImg<float> x(1, 8, 1, 1, 0); // 单应矩阵的参数向量
	CImg<float> b(1, 2 * size, 1, 1, 0);
	CImg<float> A(8, 2 * size, 1, 1, 0);

	for (int i = 0; i < randomPairs.size(); i++) {
		float x1 = randomPairs[i].a.x; float y1 = randomPairs[i].a.y;
		float x_1 = randomPairs[i].b.x; float y_1 = randomPairs[i].b.y;
		A(0, i * 2) = x1; A(1, i * 2) = y1; A(2, i * 2) = 1; A(6, i * 2) = -(x1 * x_1); A(7, i * 2) = -(y1 * x_1);
		A(3, i * 2 + 1) = x1; A(4, i * 2 + 1) = y1; A(5, i * 2 + 1) = 1; A(6, i * 2 + 1) = -(x1 * y_1); A(7, i * 2 + 1) = -(y1 * y_1);
		b(i * 2) = x_1; b(i * 2 + 1) = y_1;
		//cout << "(" << x1 << ", " << y1 << "), (" << x_1 << ", " << y_1 << ")" << endl;
	}
	// 对于Ax = b，求解x
	x = b.solve(A);
	CImg<float> H(3, 3, 1, 1, 0);
	cimg_forY(x, y) {
		H(y) = x(0, y);
	}
	H(2, 2) = 1;
	return H;
}

CImg<float> Match::RANSAC()
{
	int interation = numberOfIterations(0.99, 0.5, 4);	// 计算循环次数
	vector<int> maxInliersIndexes;						// Inlier最多的模型所包含的Inlier的下标

	for (int i = 0; i < interation; i++) {
		vector<pointPair> randomPairs;
		set<int> indexes;
		vector<int> inliersIndexes;
		// 随机取4对匹配点
		for (int j = 0; j < 4; j++) {
			int index = random(0, matchPoints.size() - 1);
			while (indexes.find(index) != indexes.end()) {
				index = random(0, matchPoints.size() - 1);
			}
			indexes.insert(index);
			randomPairs.push_back(matchPoints[index]);
		}
		// 从4对匹配点中计算单应矩阵H
		CImg<float> Homography = calculateHomography(randomPairs);
		// 对每一对匹配点，使用H计算匹配点的位置，并计算与其与实际匹配点的距离，统计Inlier的数量
		for (int j = 0; j < matchPoints.size(); j++) {
			float x1 = matchPoints[j].a.x; float y1 = matchPoints[j].a.y;
			float x_1 = matchPoints[j].b.x; float y_1 = matchPoints[j].b.y;
			CImg<float> xy1(1, 3, 1, 1, 1);
			xy1(0) = x1;
			xy1(1) = y1;
			CImg<float> target = Homography * xy1;
			target /= target(2);
			float target_x = target(0);
			float target_y = target(1);
			// 计算距离，统计Inlier
			float distance = sqrt(pow(target_x - x_1, 2) + pow(target_y - y_1, 2));
			if (distance < 1) {
				inliersIndexes.push_back(j);
			}
		}
		// 找出Inlier最多的模型
		if (inliersIndexes.size() > maxInliersIndexes.size()) {
			maxInliersIndexes = inliersIndexes;
			H = Homography;
		}
	}
	for (int i = 0; i < maxInliersIndexes.size(); i++) {
		realMatchPoints.push_back(matchPoints[maxInliersIndexes[i]]);
	}
	return H;
}