#include "Warping.h"

Warping::Warping(vector<Point> _point, CImg<unsigned char> _src) {
	intersections = _point;
	src = _src;
	dst.assign(2480, 3508, 1, 3);	// 标准A4纸大小
	H.assign(3, 3, 1, 1, 0);
}
void Warping::getPointPair() {
	// 对角点进行排序
	for (int i = 0; i < 3; i++) {
		for (int j = i + 1; j < 4; j++) {
			if (intersections[i].b > intersections[j].b) {
				Point temp = intersections[i];
				intersections[i] = intersections[j];
				intersections[j] = temp;
			}
		}
	}
	if (intersections[0].a > intersections[1].a) {
		Point temp = intersections[0];
		intersections[0] = intersections[1];
		intersections[1] = temp;
	}
	if (intersections[2].a > intersections[3].a) {
		Point temp = intersections[2];
		intersections[2] = intersections[3];
		intersections[3] = temp;
	}
	// 计算A4纸水平和垂直的长度，确定A4纸的方向
	double lengthHorizontal = sqrt(pow(intersections[0].a - intersections[1].a, 2) + pow(intersections[0].b - intersections[1].b, 2));
	double lengthVertical = sqrt(pow(intersections[0].a - intersections[2].a, 2) + pow(intersections[0].b - intersections[2].b, 2));
	if (lengthVertical > lengthHorizontal) {
		pointPairs.push_back(pointPair(Point(0,0,0), intersections[0]));
		pointPairs.push_back(pointPair(Point(2479,0,0), intersections[1]));
		pointPairs.push_back(pointPair(Point(0,3507,0), intersections[2]));
		pointPairs.push_back(pointPair(Point(2479,3507,0), intersections[3]));
	}
	else if (intersections[1].b < intersections[0].b) {
		pointPairs.push_back(pointPair(Point(0,0,0), intersections[1]));
		pointPairs.push_back(pointPair(Point(2479,0,0), intersections[3]));
		pointPairs.push_back(pointPair(Point(0,3507,0), intersections[0]));
		pointPairs.push_back(pointPair(Point(2479,3507,0), intersections[2]));
	}
	else {
		pointPairs.push_back(pointPair(Point(0,0,0), intersections[2]));
		pointPairs.push_back(pointPair(Point(2479,0,0), intersections[0]));
		pointPairs.push_back(pointPair(Point(0,3507,0), intersections[3]));
		pointPairs.push_back(pointPair(Point(2479,3507,0), intersections[1]));
	}

}
void Warping::calculateHomography() {
	int size = pointPairs.size();
	CImg<float> x(1, 8, 1, 1, 0); // 单应矩阵的参数向量
	CImg<float> b(1, 2 * size, 1, 1, 0);
	CImg<float> A(8, 2 * size, 1, 1, 0);
	ofstream out;
	out.open("corner.txt", std::ios::app);

	for (int i = 0; i < pointPairs.size(); i++) {
		float x1 = pointPairs[i].a.a; float y1 = pointPairs[i].a.b;
		float x_1 = pointPairs[i].b.a; float y_1 = pointPairs[i].b.b;
		A(0, i * 2) = x1; 
		A(1, i * 2) = y1; 
		A(2, i * 2) = 1; 
		A(6, i * 2) = -(x1 * x_1); 
		A(7, i * 2) = -(y1 * x_1);
		A(3, i * 2 + 1) = x1; 
		A(4, i * 2 + 1) = y1; 
		A(5, i * 2 + 1) = 1; 
		A(6, i * 2 + 1) = -(x1 * y_1); 
		A(7, i * 2 + 1) = -(y1 * y_1);
		b(i * 2) = x_1; b(i * 2 + 1) = y_1;
		cout << "(" << x1 << ", " << y1 << "), (" << x_1 << ", " << y_1 << ")" << endl;

		if (out.is_open()) {
			out << " " << x_1 << ", " << y_1;
		}
	}
	if (out.is_open()) {
		out << endl;
		out.close();
	}
	// 对于Ax = b，求解x
	x = b.solve(A);
	cimg_forY(x, y) {
		H(y) = x(0, y);
	}
	H(2, 2) = 1;
}
CImg<unsigned char> Warping::warp() {
	cimg_forXY(dst, x, y) {
		CImg<float> xy1(1, 3, 1, 1, 1);
		xy1(0) = x;
		xy1(1) = y;
		CImg<float> target = H * xy1;	// 反向映射
		target /= target(2);
		int target_x = (int)target(0);
		int target_y = (int)target(1);
		dst(x, y, 0) = src(target_x, target_y, 0);
		dst(x, y, 1) = src(target_x, target_y, 1);
		dst(x, y, 2) = src(target_x, target_y, 2);
	}
	return dst;
}